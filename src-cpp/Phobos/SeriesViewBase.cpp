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
  QObject::connect(&imageCache, &icache::Cache::updateImage, this, &SeriesViewBase::updateImage);
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
  auto const thumbs = imageCache.getImages(series->uuid());

  for (pcontainer::ItemPtr const& item : *series)
  {
    auto const& itemId = item->id();
    PhotoItem* widget = new PhotoItem(item, utils::asserted::fromMap(thumbs, itemId), addons, CapabilityType::REMOVE_PHOTO);
    widget->setMetrics(imageCache.metrics().get(itemId));

    QObject::connect(widget, &PhotoItem::changeSeriesState, this, &SeriesViewBase::changeCurrentSeriesState);
    QObject::connect(widget, &PhotoItem::removeFromSeries, &seriesSet, &pcontainer::Set::removeImage);

    addToLayout(widget);
  }

  currentSeriesUuid = series->uuid();
}

void SeriesViewBase::updateImage(pcontainer::ItemId const& itemId, QImage image)
{
  widgets::pitem::PhotoItem* item = findItemWidget(itemId);
  if (item)
    item->setImage(image);
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
