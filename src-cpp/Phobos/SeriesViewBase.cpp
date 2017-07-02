#include <functional>
#include "SeriesViewBase.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/Addon.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ImageCache/Cache.h"

namespace phobos {

SeriesViewBase::SeriesViewBase(icache::Cache const& imageCache) :
    imageCache(imageCache)
{
  QObject::connect(&imageCache, &icache::Cache::updateImage, this, &SeriesViewBase::updateImage);
  QObject::connect(&imageCache, &icache::Cache::updateMetrics, this, &SeriesViewBase::updateMetrics);
}

void SeriesViewBase::showSeries(pcontainer::SeriesPtr const& series)
{
    using namespace widgets::pitem;
    clear();

    auto const& addons = Addons(config::get()->get_qualified_array_of<std::string>("seriesView.enabledAddons").value_or({}));

    for (pcontainer::ItemPtr const& item : *series)
    {
        PhotoItem* widget = new PhotoItem(item, imageCache.getImage(item->id()), addons, CapabilityType::REMOVE_PHOTO);

        QObject::connect(widget, &PhotoItem::changeSeriesState,
                         this, &SeriesViewBase::changeCurrentSeriesState);

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
