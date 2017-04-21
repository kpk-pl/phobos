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

    QImage buildPreloadImage()
    {
        QPixmap pixmap(preferredSize());
        pixmap.fill(Qt::lightGray);
        return pixmap.toImage();
    }

    QImage getPreloadImage()
    {
        // TODO: handle when size changes
        static QImage const image = buildPreloadImage();
        return image;
    }
} // unnamed namespace

void SeriesViewBase::showSeries(pcontainer::SeriesPtr const& series)
{
    clear();

    currentSeriesUuid = series->uuid();
    for (pcontainer::ItemPtr const& item : *series)
    {
        auto const& preload = (item->hasImage() ? item->image() : getPreloadImage());
        PhotoItemWidget* widget = new PhotoItemWidget(item, preload,
            PhotoItemWidgetAddons(config::get()->get_qualified_array_of<std::string>("seriesView.enabledAddons").value_or({})));
        addToLayout(widget);
        item->loadPhoto(preferredSize(), widget, std::bind(&PhotoItemWidget::setImage, widget, std::placeholders::_1));
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