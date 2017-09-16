#include <functional>
#include "SeriesViewBase.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/Addon.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ImageCache/Cache.h"
#include "Utils/Asserted.h"

namespace phobos {

SeriesViewBase::SeriesViewBase(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
    seriesSet(seriesSet), imageCache(imageCache)
{
  QObject::connect(&imageCache, &icache::Cache::updateMetrics, this, &SeriesViewBase::updateMetrics);
  QObject::connect(&seriesSet, &pcontainer::Set::changedSeries, this, &SeriesViewBase::updateSeries);
}

// TODO: maybe if at all possible. Ctrl+Arrow jump focus to best photo in series
// TODO: This function has a lot in common with AllSeriesView -> derive from common base
void SeriesViewBase::showSeries(pcontainer::SeriesPtr const& series)
{
  using namespace widgets::pitem;
  assert(series);
  clear();

  auto const& addons = Addons(config::get()->get_qualified_array_of<std::string>("seriesView.enabledAddons").value_or({}));

  for (pcontainer::ItemPtr const& item : *series)
  {
    auto const& itemId = item->id();
    auto widget = std::make_unique<PhotoItem>(item, addons, CapabilityType::REMOVE_PHOTO);

    auto result = imageCache.transaction().item(itemId).callback([lt=widget->lifetime()](auto && res){
      auto item = lt.lock();
      if (item) item->setImage(res.image);
    }).execute();

    widget->setImage(result.image);
    widget->setMetrics(imageCache.metrics().get(itemId));

    QObject::connect(widget.get(), &PhotoItem::changeSeriesState, this, &SeriesViewBase::changeCurrentSeriesState);
    QObject::connect(widget.get(), &PhotoItem::removeFromSeries, &seriesSet, &pcontainer::Set::removeImage);

    addToLayout(widget.release());
  }

  currentSeriesUuid = series->uuid();
}

void SeriesViewBase::updateMetrics(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics)
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
