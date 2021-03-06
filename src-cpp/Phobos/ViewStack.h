#ifndef VIEWSTACK_H
#define VIEWSTACK_H

#include <boost/optional.hpp>
#include <QStackedWidget>
#include <QUuid>
#include "ViewDescription.h"
#include "PhotoBulkAction.h"
#include "SharedWidgets.h"
#include "ImportWizard/Types.h"
#include "Views/Fwd.h"
#include "PhotoContainers/Fwd.h"
#include "PhotoContainers/ItemId.h"
#include "ImageCache/CacheFwd.h"
#include "ImageProcessing/Enhance/OperationType.h"

namespace phobos {

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
  void saveItemInLaboratory(QString const& fileName) const;

public slots:
  void handleSwitchView(ViewDescriptionPtr viewDesc);
  void showImageFullscreen(pcontainer::ItemId const& itemId);
  void bulkSelect(PhotoBulkAction const action);
  void photoEnhancement(iprocess::enhance::OperationType const operation);

private slots:
  void welcomeScreenSwitch();

private:
  pcontainer::Series const& findRequestedSeries(ViewDescriptionPtr const& viewDesc) const;
  pcontainer::Item const& findRequestedPhoto(pcontainer::Series const& requestedSeries, int photoOffset);

  void switchToAllSeries(pcontainer::Series const& series);
  void switchToLaboratory(pcontainer::Item const& item);

  void setupUI();
  void connectSignals();

  pcontainer::Set const& seriesSet;
  icache::Cache & imageCache;
  SharedWidgets const& sharedWidgets;
  MainToolbar* const mainToolbar;

  view::Welcome* welcomeView;
  view::AllSeries* allSeriesView;
  view::RowSeries* rowSeriesView;
  view::NumSeries* numSeriesView;
  view::Laboratory* laboratoryView;

  view::SeriesBase* currentSeriesWidget;
};

} // namespace phobos

#endif // VIEWSTACK_H
