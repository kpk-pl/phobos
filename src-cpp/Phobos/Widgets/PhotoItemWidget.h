#ifndef WIDGETS_PHOTOITEMWIDGET_H
#define WIDGETS_PHOTOITEMWIDGET_H

#include <memory>
#include <QUuid>
#include "Widgets/ImageWidget.h"
#include "Widgets/PhotoItemWidgetAddon.h"
#include "PhotoContainers/Item.h"

namespace phobos { namespace widgets {

class PhotoItemWidget : public ImageWidget
{
    Q_OBJECT
public:
    explicit PhotoItemWidget(pcontainer::ItemPtr const& photoItem,
                             QImage const& preload,
                             PhotoItemWidgetAddons const& addons);

    pcontainer::Item const& photoItem() const { return *_photoItem; }

    void contextMenuEvent(QContextMenuEvent* event) override;
    void focusInEvent(QFocusEvent*) override;
    void focusOutEvent(QFocusEvent*) override;
    bool eventFilter(QObject* object, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent*) override;

signals:
    void openInSeries(QUuid const seriesUuid);
    void changeSeriesState(QUuid const seriesUuid, pcontainer::ItemState);

private:
    class PixmapRenderer;
    friend class PixmapRenderer;

    pcontainer::ItemPtr const _photoItem;
    PhotoItemWidgetAddons const addons;
};

}} // namespace phobos::widgets

#endif // WIDGETS_PHOTOITEMWIDGET_H
