#ifndef ALLSERIESVIEW_H
#define ALLSERIESVIEW_H

#include <map>
#include <tuple>
#include <QWidget>
#include <boost/optional.hpp>
#include "PhotoContainers/Item.h"
#include "ImageCache/CacheFwd.h"
#include "ViewDescription.h"
#include "ImageProcessing/MetricsFwd.h"

class QGridLayout;

namespace phobos {

namespace widgets { namespace pitem {
class PhotoItem;
}} // namespace widgets::pitem

class AllSeriesView : public QWidget
{
    Q_OBJECT
public:
    explicit AllSeriesView(pcontainer::Set const& seriesSet, icache::Cache const& imageCache);

    std::size_t numberOfSeries() const { return seriesUuidToRow.size(); }

    void keyPressEvent(QKeyEvent* keyEvent) override;

    void focusSeries();
    void focusSeries(QUuid const seriesUuid);

signals:
    void switchView(ViewDescriptionPtr viewDesc);

public slots:
    void addNewSeries(pcontainer::SeriesPtr series);
    void updateExistingSeries(QUuid seriesUuid);

private slots:
    void changeSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state);
    void updateImage(pcontainer::ItemId const& itemId);
    void updateMetrics(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics);

private:
    struct Coords;
    boost::optional<Coords> focusGridCoords() const;
    std::vector<Coords> nextJumpProposals(Coords const& coords,
                                          int const directionKey) const;
    Coords findValidProposal(std::vector<Coords> const& proposals) const;

    widgets::pitem::PhotoItem* findItem(pcontainer::ItemId const& itemId) const;

    void addItemToGrid(int row, int col, pcontainer::ItemPtr const& itemPtr, QImage const& thumbnail);

    pcontainer::Set const& seriesSet;
    icache::Cache const& imageCache;
    std::map<QUuid, std::size_t> seriesUuidToRow;
    QGridLayout* grid;
};

} // namespace phobos

#endif // ALLSERIESVIEW_H
