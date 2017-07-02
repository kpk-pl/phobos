#ifndef SERIESVIEWBASE_H
#define SERIESVIEWBASE_H

#include <QWidget>
#include <QUuid>
#include <QMetaObject>
#include <boost/optional.hpp>
#include "PhotoContainers/Series.h"
#include "ImageCache/CacheFwd.h"
#include "ViewDescription.h"

namespace phobos {

namespace widgets {
class PhotoItemWidget;
} // namespace widgets

class SeriesViewBase : public QWidget
{
    Q_OBJECT
public:
    explicit SeriesViewBase(icache::Cache const& imageCache);
    virtual ~SeriesViewBase() = default;

    virtual void showSeries(pcontainer::SeriesPtr const& series);
    virtual void clear();

signals:
    void switchView(ViewDescriptionPtr viewDesc);

protected:
    virtual void addToLayout(widgets::PhotoItemWidget* itemWidget) = 0;
    virtual void changeSeriesState(pcontainer::ItemState const state) const = 0;

    icache::Cache const& imageCache;
    boost::optional<QUuid> currentSeriesUuid;

private slots:
    void changeCurrentSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state);
};

} // namespace phobos

#endif // SERIESVIEWBASE_H
