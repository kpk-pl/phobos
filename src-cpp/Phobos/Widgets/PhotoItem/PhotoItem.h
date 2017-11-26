#ifndef WIDGETS_PHOTOITEM_PHOTOITEM_H
#define WIDGETS_PHOTOITEM_PHOTOITEM_H

#include <memory>
#include <QUuid>
#include "Widgets/ImageWidget.h"
#include "Widgets/PhotoItem/Addon.h"
#include "Widgets/PhotoItem/Capability.h"
#include "PhotoContainers/Item.h"

namespace phobos { namespace widgets { namespace pitem {

class PhotoItem : public ImageWidget
{
    Q_OBJECT
public:
    explicit PhotoItem(pcontainer::ItemPtr const& photoItem,
                       Addons const& addons,
                       Capabilities const& capabilities);

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
    void removeFromSeries(pcontainer::ItemId itemId);
    void removeAllSeries(QUuid const seriesUuid);

public slots:
    void openDetailsDialog() const;

private:
    class PixmapRenderer;
    friend class PixmapRenderer;

    pcontainer::ItemPtr const _photoItem;
    Addons const addons;
    Capabilities const capabilities;
};

}}} // namespace phobos::widgets::pitem

#endif // WIDGETS_PHOTOITEM_PHOTOITEM_H
