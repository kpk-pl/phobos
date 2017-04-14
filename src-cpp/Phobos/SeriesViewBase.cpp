#include <functional>
#include "SeriesViewBase.h"
#include "PhotoItemWidget.h"
#include "PhotoItemWidgetAddon.h"
#include "Config.h"
#include "ConfigExtension.h"

namespace phobos {

namespace {
    QSize preferredSize()
    {
        return config::qSize("seriesView.maxPixmapSize").value_or(QSize(1920, 1080));
    }

    std::shared_ptr<QPixmap> const& getPreloadPixmap()
    {
        // TODO: handle when size changes
        static auto result = std::make_shared<QPixmap>(preferredSize());
        result->fill(Qt::lightGray);
        return result;
    }
} // unnamed namespace

void SeriesViewBase::showSeries(pcontainer::SeriesPtr const& series)
{
    clear();

    currentSeriesUuid = series->uuid();
    for (pcontainer::ItemPtr const& item : series->items())
    {
        auto const& preload = (item->hasPixmap() ? item->pixmap() : getPreloadPixmap());
        PhotoItemWidget* widget = new PhotoItemWidget(item, preload,
            PhotoItemWidgetAddons(config::get()->get_qualified_array_of<std::string>("seriesView.enabledAddons").value_or({})));
        addToLayout(widget);
        item->loadPhoto(preferredSize(), widget, std::bind(&PhotoItemWidget::setImagePixmap, widget, std::placeholders::_1));
    }
}

void SeriesViewBase::moveItemsIn(std::vector<PhotoItemWidget*> const& items)
{
    for (auto const& item : items)
        addToLayout(item);
    currentSeriesUuid = items.front()->photoItem().seriesUuid();
}

void SeriesViewBase::exchangeItemsFrom(SeriesViewBase *source)
{
    moveItemsIn(source->moveItemsOut());
}

void SeriesViewBase::clear()
{
    currentSeriesUuid.reset();
}

} // namespace phobos
