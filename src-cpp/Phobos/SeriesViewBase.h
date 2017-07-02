#ifndef SERIESVIEWBASE_H
#define SERIESVIEWBASE_H

#include <QWidget>
#include <QUuid>
#include <QMetaObject>
#include <boost/optional.hpp>
#include "PhotoContainers/Series.h"
#include "PhotoContainers/ItemId.h"
#include "ImageCache/CacheFwd.h"
#include "ViewDescription.h"
#include "ImageProcessing/MetricsFwd.h"

namespace phobos {

namespace widgets { namespace pitem {
class PhotoItem;
}} // namespace widgets::pitem

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
    virtual void addToLayout(widgets::pitem::PhotoItem* itemWidget) = 0;
    virtual void changeSeriesState(pcontainer::ItemState const state) const = 0;

    icache::Cache const& imageCache;
    boost::optional<QUuid> currentSeriesUuid;

private slots:
    void changeCurrentSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state);
    void updateImage(pcontainer::ItemId const& itemId, QImage image);
    void updateMetrics(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics);

private:
    virtual widgets::pitem::PhotoItem*
        findItemWidget(pcontainer::ItemId const& itemId) const = 0;
};

} // namespace phobos

#endif // SERIESVIEWBASE_H
