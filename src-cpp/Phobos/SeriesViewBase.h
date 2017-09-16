#ifndef SERIESVIEWBASE_H
#define SERIESVIEWBASE_H

#include <QWidget>
#include <QUuid>
#include <QMetaObject>
#include <boost/optional.hpp>
#include "PhotoContainers/Fwd.h"
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
    explicit SeriesViewBase(pcontainer::Set const& seriesSet, icache::Cache & imageCache);
    virtual ~SeriesViewBase() = default;

    virtual void showSeries(pcontainer::SeriesPtr const& series);
    virtual void clear();

signals:
    void switchView(ViewDescriptionPtr viewDesc);

protected:
    virtual void addToLayout(widgets::pitem::PhotoItem* itemWidget) = 0;
    virtual void changeSeriesState(pcontainer::ItemState const state) const = 0;
    virtual widgets::pitem::PhotoItem*
        findItemWidget(pcontainer::ItemId const& itemId) const = 0;

    virtual void updateCurrentSeries() = 0;

    pcontainer::Set const& seriesSet;
    icache::Cache & imageCache;
    boost::optional<QUuid> currentSeriesUuid;

private slots:
    void changeCurrentSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state);
    void updateMetrics(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics);
    void updateSeries(QUuid seriesUuid);
};

} // namespace phobos

#endif // SERIESVIEWBASE_H
