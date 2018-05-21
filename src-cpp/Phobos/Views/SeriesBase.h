#ifndef SERIESVIEWBASE_H
#define SERIESVIEWBASE_H

#include "PhotoContainers/Series.h"
#include "PhotoContainers/ItemId.h"
#include "ViewDescription.h"
#include "ImageProcessing/MetricsFwd.h"
#include "Views/View.h"
#include <boost/optional.hpp>
#include <QWidget>
#include <QUuid>
#include <QMetaObject>
#include <map>

class QLayout;

namespace phobos {

namespace widgets { namespace pitem {
class PhotoItem;
}} // namespace widgets::pitem

namespace view {

class SeriesBase : public View
{
Q_OBJECT
public:
  explicit SeriesBase(pcontainer::Set const& seriesSet, icache::Cache & imageCache);
  virtual ~SeriesBase() = default;

  virtual void showSeries(pcontainer::Series const& series);
  virtual void clear();

  boost::optional<QUuid> const& seriesUuid() const { return currentSeriesUuid; }

signals:
    void switchView(ViewDescriptionPtr viewDesc);
    void showImageFullscreen(pcontainer::ItemId);

protected:
    virtual void addToLayout(std::unique_ptr<widgets::pitem::PhotoItem> itemWidget);
    virtual void changeSeriesState(pcontainer::ItemState const state) const = 0;
    virtual widgets::pitem::PhotoItem*
        findItemWidget(pcontainer::ItemId const& itemId) const = 0;

    std::unique_ptr<widgets::pitem::PhotoItem> createConnectedItem(pcontainer::ItemPtr const& item);

    virtual QLayout* getLayoutForItems() const = 0;

    virtual void updateCurrentSeries() = 0;
    void updateCurrentSeriesFromContent(
        std::map<pcontainer::ItemId, std::unique_ptr<widgets::pitem::PhotoItem>> &content);

    boost::optional<QUuid> currentSeriesUuid;

private slots:
    void changeCurrentSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state);
    void updateMetrics(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics);
    void updateSeries(QUuid seriesUuid);
};

}} // namespace phobos::view

#endif // SERIESVIEWBASE_H
