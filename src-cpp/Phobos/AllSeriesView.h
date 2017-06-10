#ifndef ALLSERIESVIEW_H
#define ALLSERIESVIEW_H

#include <map>
#include <tuple>
#include <QWidget>
#include <boost/optional.hpp>
#include "PhotoContainers/Series.h"
#include "ImageCache/CacheFwd.h"
#include "ViewDescription.h"
#include "ImageProcessing/MetricsFwd.h"

class QGridLayout;

namespace phobos {

class PhotoItemWidget;

class AllSeriesView : public QWidget
{
    Q_OBJECT
public:
    explicit AllSeriesView(icache::Cache const& imageCache);

    std::size_t numberOfSeries() const { return seriesUuidToRow.size(); }

    void keyPressEvent(QKeyEvent* keyEvent) override;

    void focusSeries();
    void focusSeries(QUuid const seriesUuid);

signals:
    void switchView(ViewDescriptionPtr viewDesc);

public slots:
    void addNewSeries(pcontainer::SeriesPtr series);

private slots:
    void changeSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state);
    void updateImage(QUuid seriesUuid, QString fileName);
    void updateMetrics(QUuid seriesUuid, QString fileName, iprocess::MetricPtr metrics);

private:
    struct Coords;
    boost::optional<Coords> focusGridCoords() const;
    std::vector<Coords> nextJumpProposals(Coords const& coords,
                                          int const directionKey) const;
    Coords findValidProposal(std::vector<Coords> const& proposals) const;

    PhotoItemWidget* findItem(QUuid const& seriesUuid, std::string const& filename) const;

    icache::Cache const& imageCache;
    std::map<QUuid, std::size_t> seriesUuidToRow;
    QGridLayout* grid;
};

} // namespace phobos

#endif // ALLSERIESVIEW_H
