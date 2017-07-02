#include <functional>
#include "SeriesViewBase.h"
#include "Widgets/PhotoItemWidget.h"
#include "Widgets/PhotoItemWidgetAddon.h"
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

    auto const& addons = widgets::PhotoItemWidgetAddons(config::get()->get_qualified_array_of<std::string>("seriesView.enabledAddons").value_or({}));

    for (pcontainer::ItemPtr const& item : *series)
    {
        widgets::PhotoItemWidget* widget = new widgets::PhotoItemWidget(item, imageCache.getImage(*item), addons);

        QObject::connect(widget, &widgets::PhotoItemWidget::changeSeriesState,
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
