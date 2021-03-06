#ifndef ALLSERIESVIEW_H
#define ALLSERIESVIEW_H

#include "PhotoContainers/Item.h"
#include "ViewDescription.h"
#include "Views/View.h"
#include "ImageProcessing/MetricsFwd.h"
#include <boost/optional.hpp>
#include <QWidget>
#include <map>

class QGridLayout;
class QScrollArea;

namespace phobos {

namespace widgets { namespace pitem {
class PhotoItem;
}} // namespace widgets::pitem

namespace view {

class AllSeries : public View
{
    Q_OBJECT
public:
    explicit AllSeries(pcontainer::Set const& seriesSet, icache::Cache & imageCache);

    std::size_t numberOfSeries() const { return seriesUuidToRow.size(); }

    void keyPressEvent(QKeyEvent* keyEvent) override;

    void focusSeries();
    void focusSeries(QUuid const seriesUuid);

signals:
    void switchView(ViewDescriptionPtr viewDesc);
    void showImageFullscreen(pcontainer::ItemId);

public slots:
    void addNewSeries(pcontainer::SeriesPtr series);
    void updateExistingSeries(QUuid seriesUuid);

private slots:
    void changeSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state);
    void updateMetrics(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics);

private:
    struct Coords;
    boost::optional<Coords> focusGridCoords() const;
    std::vector<Coords> nextJumpProposals(Coords const& coords,
                                          int const directionKey) const;
    Coords findValidProposal(std::vector<Coords> const& proposals) const;

    widgets::pitem::PhotoItem* findItem(pcontainer::ItemId const& itemId) const;

    std::size_t maxNumberOfPhotosInRow() const;
    QWidget* photoInGridAt(std::size_t const row, std::size_t const col) const;
    void addPhotoToGridAt(QWidget *widget, std::size_t const row, std::size_t const col);

    void addItemToGrid(int const row, int const col, pcontainer::ItemPtr const& itemPtr);
    void addNumberingToGrid(int const row);

    void setFocusedWidget(QWidget *widget) const;

    void prepareUI();

    std::map<QUuid, std::size_t> seriesUuidToRow;
    QScrollArea* scroll;
    QGridLayout* grid;
};

}} // namespace phobos::view

#endif // ALLSERIESVIEW_H
