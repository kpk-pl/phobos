#ifndef ALLSERIESVIEW_H
#define ALLSERIESVIEW_H

#include <map>
#include <tuple>
#include <QWidget>
#include <boost/optional.hpp>
#include "PhotoContainers/Series.h"
#include "ViewDescription.h"

class QGridLayout;

namespace phobos {

class AllSeriesView : public QWidget
{
    Q_OBJECT
public:
    explicit AllSeriesView();

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

private:
    struct Coords;
    boost::optional<Coords> focusGridCoords() const;
    std::vector<Coords> nextJumpProposals(Coords const& coords,
                                          int const directionKey) const;
    Coords findValidProposal(std::vector<Coords> const& proposals) const;

    std::map<QUuid, std::size_t> seriesUuidToRow;
    QGridLayout* grid;
};

} // namespace phobos

#endif // ALLSERIESVIEW_H
