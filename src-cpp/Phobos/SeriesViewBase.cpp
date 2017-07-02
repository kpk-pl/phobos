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
}

void SeriesViewBase::showSeries(pcontainer::SeriesPtr const& series)
{
    clear();

    auto const& addons = widgets::pitem::Addons(config::get()->get_qualified_array_of<std::string>("seriesView.enabledAddons").value_or({}));

    for (pcontainer::ItemPtr const& item : *series)
    {
        widgets::pitem::PhotoItem* widget = new widgets::pitem::PhotoItem(item, imageCache.getImage(*item), addons);

        QObject::connect(widget, &widgets::pitem::PhotoItem::changeSeriesState,
                         this, &SeriesViewBase::changeCurrentSeriesState);

        addToLayout(widget);
    }

    currentSeriesUuid = series->uuid();
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
