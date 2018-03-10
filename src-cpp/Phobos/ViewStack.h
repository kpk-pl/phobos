#ifndef VIEWSTACK_H
#define VIEWSTACK_H

#include <boost/optional.hpp>
#include <QStackedWidget>
#include <QUuid>
#include "ViewDescription.h"
#include "PhotoBulkAction.h"
#include "SharedWidgets.h"
#include "ImportWizard/Types.h"
#include "PhotoContainers/Fwd.h"
#include "PhotoContainers/ItemId.h"
#include "ImageCache/CacheFwd.h"
#include "ImageProcessing/Enhance/OperationType.h"

namespace phobos {

class AllSeriesView;
class NumSeriesView;
class RowSeriesView;
class LaboratoryView;
class WelcomeView;
class SeriesViewBase;
class MainToolbar;

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

  explicit ViewStack(pcontainer::Set const& seriesSet,
                     icache::Cache &cache,
                     SharedWidgets const& sharedWidgets,
                     MainToolbar *MainToolbar);

  SelectionStatus getSelectionStatus() const;
  void setCurrentWidget(QWidget *widget);

  boost::optional<pcontainer::ItemId> currentItemInLaboratory() const;

public slots:
  void handleSwitchView(ViewDescriptionPtr viewDesc);
  void bulkSelect(PhotoBulkAction const action);
  void photoEnhancement(iprocess::enhance::OperationType const operation);

private slots:
  void welcomeScreenSwitch();

private:
  pcontainer::Series const& findRequestedSeries(boost::optional<QUuid> const& requestedSeries, int const seriesOffset) const;
  pcontainer::Item const& findRequestedPhoto(pcontainer::Series const& requestedSeries, int photoOffset);

  void switchToAllSeries(pcontainer::Series const& series);
  void switchToLaboratory(pcontainer::Item const& item);

  void setupUI();
  void connectSignals();

  pcontainer::Set const& seriesSet;
  icache::Cache & imageCache;
  SharedWidgets const& sharedWidgets;
  MainToolbar* const mainToolbar;

  WelcomeView* welcomeView;
  AllSeriesView* allSeriesView;
  RowSeriesView* rowSeriesView;
  NumSeriesView* numSeriesView;
  LaboratoryView* laboratoryView;

  SeriesViewBase* currentSeriesWidget;
};

} // namespace phobos

#endif // VIEWSTACK_H
