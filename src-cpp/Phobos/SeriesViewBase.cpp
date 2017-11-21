#include "SeriesViewBase.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/Addon.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ImageCache/Cache.h"
#include "Utils/Asserted.h"
#include <easylogging++.h>
#include <functional>
#include <QLayout>

namespace phobos {

SeriesViewBase::SeriesViewBase(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
    seriesSet(seriesSet), imageCache(imageCache)
{
  QObject::connect(&imageCache, &icache::Cache::updateMetrics, this, &SeriesViewBase::updateMetrics);
  QObject::connect(&seriesSet, &pcontainer::Set::changedSeries, this, &SeriesViewBase::updateSeries);
}

// TODO: maybe if at all possible. Ctrl+Arrow jump focus to best photo in series
// TODO: This function has a lot in common with AllSeriesView -> derive from common base

std::unique_ptr<widgets::pitem::PhotoItem> SeriesViewBase::createConnectedItem(pcontainer::ItemPtr const& item)
{
  using namespace widgets::pitem;

  auto const& addons = Addons(config::get()->get_qualified_array_of<std::string>("seriesView.enabledAddons").value_or({}));
  auto const& itemId = item->id();
  auto widget = std::make_unique<PhotoItem>(item, addons, CapabilityType::REMOVE_PHOTO);

  auto result = imageCache.transaction().item(itemId).callback([lt=widget->lifetime()](auto && res){
    auto item = lt.lock();
    if (item) item->setImage(res.image);
  }).proactive().execute();

  widget->setBorder(config::qualified("photoItemWidget.border.width", 0));
  widget->setImage(result.image);
  widget->setMetrics(imageCache.metrics().get(itemId));

  QObject::connect(widget.get(), &PhotoItem::changeSeriesState, this, &SeriesViewBase::changeCurrentSeriesState);
  QObject::connect(widget.get(), &PhotoItem::removeFromSeries, &seriesSet, &pcontainer::Set::removeImage);

  return widget;
}

void SeriesViewBase::addToLayout(std::unique_ptr<widgets::pitem::PhotoItem> itemWidget)
{
  getLayoutForItems()->addWidget(itemWidget.release());
}

void SeriesViewBase::showSeries(pcontainer::Series const& series)
{
  using namespace widgets::pitem;
  clear();

  for (pcontainer::ItemPtr const& item : series)
    addToLayout(createConnectedItem(item));

  currentSeriesUuid = series.uuid();

  if (!series.empty())
    getLayoutForItems()->itemAt(0)->widget()->setFocus();
}

void SeriesViewBase::updateCurrentSeriesFromContent(
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

void SeriesViewBase::updateMetrics(pcontainer::ItemId const& itemId,
                                   iprocess::metric::MetricPtr metrics)
{
  widgets::pitem::PhotoItem* item = findItemWidget(itemId);
  if (item)
    item->setMetrics(metrics);
}

void SeriesViewBase::updateSeries(QUuid seriesUuid)
{
  if (currentSeriesUuid != seriesUuid)
    return;

  updateCurrentSeries();
}

void SeriesViewBase::clear()
{
  currentSeriesUuid.reset();
}

void SeriesViewBase::changeCurrentSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state)
{
  assert(currentSeriesUuid);
  assert(*currentSeriesUuid == seriesUuid);

  changeSeriesState(state);
}

} // namespace phobos
