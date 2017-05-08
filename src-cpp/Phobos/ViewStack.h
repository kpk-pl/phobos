#ifndef VIEWSTACK_H
#define VIEWSTACK_H

#include <boost/optional.hpp>
#include <QStackedWidget>
#include <QUuid>
#include "PhotoContainers/Set.h"
#include "ViewDescription.h"
#include "PhotoBulkAction.h"
#include "ImportWizard/Types.h"

namespace phobos {

class AllSeriesView;
class NumSeriesView;
class RowSeriesView;
class SeriesViewBase;

class ViewStack : public QStackedWidget
{
    Q_OBJECT

public:
    struct SelectionStatus
    {
        struct SeriesSelectionStatus
        {
            std::vector<std::string> selected;
            std::vector<std::string> discarded;
            std::vector<std::string> others;
        };
        std::vector<SeriesSelectionStatus> status;
    };

    explicit ViewStack();
    void addPhotos(importwiz::PhotoSeriesVec const& photoSeries);
    SelectionStatus getSelectionStatus() const;

public slots:
    void handleSwitchView(ViewDescriptionPtr viewDesc);
    void bulkSelect(PhotoBulkAction const action);

private:
    pcontainer::SeriesPtr const& findRequestedSeries(ViewDescriptionPtr const& viewDesc) const;

    void setupUI();
    void connectSignals();

    pcontainer::Set seriesSet;
    boost::optional<QUuid> currentSeriesInView;

    AllSeriesView* allSeriesView;
    RowSeriesView* rowSeriesView;
    NumSeriesView* numSeriesView;
    SeriesViewBase* currentSeriesWidget;
};

} // namespace phobos

#endif // VIEWSTACK_H
