#include "Views/SeriesBase.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/Addon.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ImageCache/Cache.h"
#include "Utils/Asserted.h"
#include <easylogging++.h>
#include <functional>
#include <QLayout>

namespace phobos { namespace view {

SeriesBase::SeriesBase(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
  View(seriesSet, imageCache)
{
  QObject::connect(&imageCache, &icache::Cache::updateMetrics, this, &SeriesBase::updateMetrics);
  QObject::connect(&seriesSet, &pcontainer::Set::changedSeries, this, &SeriesBase::updateSeries);
}

// TODO: maybe if at all possible. Ctrl+Arrow jump focus to best photo in series
// TODO: This function has a lot in common with AllSeriesView -> derive from common base

std::unique_ptr<widgets::pitem::PhotoItem> SeriesBase::createConnectedItem(pcontainer::ItemPtr const& item)
{
  using namespace widgets::pitem;

  auto const& addons = Addons(config::qualified("seriesView.enabledAddons", std::vector<std::string>{}));
  auto const& itemId = item->id();
  auto widget = std::make_unique<PhotoItem>(item, addons, CapabilityType::REMOVE_PHOTO);

  auto result = imageCache.transaction().item(itemId).callback([lt=widget->lifetime()](auto && res){
    auto item = lt.lock();
    if (item) item->setImage(res.image);
  }).proactive().execute();

  widget->setBorder(config::qualified("photoItemWidget.border.width", 0));
  widget->setImage(result.image);
  widget->setMetrics(imageCache.metrics().get(itemId));

  QObject::connect(widget.get(), &PhotoItem::changeSeriesState, this, &SeriesBase::changeCurrentSeriesState);
  QObject::connect(widget.get(), &PhotoItem::removeFromSeries, &seriesSet, &pcontainer::Set::removeImage);
  QObject::connect(widget.get(), &PhotoItem::showFullscreen, this, &SeriesBase::showImageFullscreen);

  return widget;
}

void SeriesBase::addToLayout(std::unique_ptr<widgets::pitem::PhotoItem> itemWidget)
{
  getLayoutForItems()->addWidget(itemWidget.release());
}

void SeriesBase::showSeries(pcontainer::Series const& series)
{
  using namespace widgets::pitem;
  clear();

  for (pcontainer::ItemPtr const& item : series)
    addToLayout(createConnectedItem(item));

  currentSeriesUuid = series.uuid();

  if (!series.empty())
    getLayoutForItems()->itemAt(0)->widget()->setFocus();

  update();
}

void SeriesBase::updateCurrentSeriesFromContent(
    std::map<pcontainer::ItemId, std::unique_ptr<widgets::pitem::PhotoItem>> &content)
{
  if (!currentSeriesUuid)
    return;

  for (auto const& item : seriesSet.findSeries(*currentSeriesUuid))
  {
    auto const oldIt = content.find(item->id());
    if (oldIt != content.end())
    {
      LOG(DEBUG) << "Adding item from saved content " << item->id().toString();
      addToLayout(std::move(oldIt->second));
      content.erase(oldIt);
    }
    else
    {
      LOG(DEBUG) << "Adding newly constructed item " << item->id().toString();
      addToLayout(createConnectedItem(item));
    }
  }
}

void SeriesBase::updateMetrics(pcontainer::ItemId const& itemId,
                                   iprocess::MetricPtr metrics)
{
  widgets::pitem::PhotoItem* item = findItemWidget(itemId);
  if (item)
    item->setMetrics(metrics);
}

void SeriesBase::updateSeries(QUuid seriesUuid)
{
  if (currentSeriesUuid != seriesUuid)
    return;

  updateCurrentSeries();
}

void SeriesBase::clear()
{
  currentSeriesUuid.reset();
}

void SeriesBase::changeCurrentSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state)
{
  assert(currentSeriesUuid);
  assert(*currentSeriesUuid == seriesUuid);

  changeSeriesState(state);
}

}} // namespace phobos::view
