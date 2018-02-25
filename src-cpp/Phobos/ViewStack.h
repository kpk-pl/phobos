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
#include "ImageCache/CacheFwd.h"

namespace phobos {

class AllSeriesView;
class NumSeriesView;
class RowSeriesView;
class LaboratoryView;
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

  explicit ViewStack(pcontainer::Set const& seriesSet,
                     icache::Cache &cache,
                     SharedWidgets const& sharedWidgets);

  SelectionStatus getSelectionStatus() const;
  void setCurrentWidget(QWidget *widget);

signals:
  void importPhotosRequest();

public slots:
  void handleSwitchView(ViewDescriptionPtr viewDesc);
  void bulkSelect(PhotoBulkAction const action);

private slots:
  void welcomeScreenSwitch();

private:
  pcontainer::Series const& findRequestedSeries(ViewDescriptionPtr const& viewDesc) const;
  pcontainer::Item const& findRequestedPhoto(pcontainer::Series const& requestedSeries, int const photoOffset);

  void switchToAllSeries(pcontainer::Series const& series);
  void switchToLaboratory(pcontainer::Item const& item);

  void setupUI();
  void connectSignals();

  pcontainer::Set const& seriesSet;
  icache::Cache & imageCache;
  SharedWidgets const& sharedWidgets;
  boost::optional<QUuid> currentSeriesInView;

  WelcomeView* welcomeView;
  AllSeriesView* allSeriesView;
  RowSeriesView* rowSeriesView;
  NumSeriesView* numSeriesView;
  LaboratoryView* laboratoryView;

  SeriesViewBase* currentSeriesWidget;
};

} // namespace phobos

#endif // VIEWSTACK_H
