#ifndef VIEWSTACK_H
#define VIEWSTACK_H

#include <boost/optional.hpp>
#include <QStackedWidget>
#include <QUuid>
#include "ViewDescription.h"
#include "PhotoBulkAction.h"
#include "ImportWizard/Types.h"
#include "PhotoContainers/Fwd.h"
#include "ImageCache/CacheFwd.h"

namespace phobos {

class AllSeriesView;
class NumSeriesView;
class RowSeriesView;
class WelcomeView;
class SeriesViewBase;

class ViewStack : public QStackedWidget
{
    Q_OBJECT

public:
    struct SelectionStatus
    {
      struct SeriesSelectionStatus
      {
        std::vector<QString> selected;
        std::vector<QString> others;
      };
      std::vector<SeriesSelectionStatus> status;
    };

    explicit ViewStack(pcontainer::Set const& seriesSet, icache::Cache & cache);
    SelectionStatus getSelectionStatus() const;

public slots:
    void handleSwitchView(ViewDescriptionPtr viewDesc);
    void bulkSelect(PhotoBulkAction const action);

private slots:
    void welcomeScreenSwitch();

private:
    pcontainer::Series const& findRequestedSeries(ViewDescriptionPtr const& viewDesc) const;

    void setupUI();
    void connectSignals();

    pcontainer::Set const& seriesSet;
    icache::Cache & imageCache;
    boost::optional<QUuid> currentSeriesInView;

    WelcomeView* welcomeView;
    AllSeriesView* allSeriesView;
    RowSeriesView* rowSeriesView;
    NumSeriesView* numSeriesView;

    SeriesViewBase* currentSeriesWidget;
};

} // namespace phobos

#endif // VIEWSTACK_H
