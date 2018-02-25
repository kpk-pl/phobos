#include "ViewStack.h"
#include "WelcomeView.h"
#include "AllSeriesView.h"
#include "NumSeriesView.h"
#include "RowSeriesView.h"
#include "LaboratoryView.h"
#include "Utils/Focused.h"
#include "ConfigExtension.h"
#include "PhotoContainers/Set.h"
#include "ImageCache/Cache.h"
#include "Widgets/StatusBarSlider.h"
#include "Widgets/StatusBarLeftRightNavigation.h"
#include "Utils/Asserted.h"
#include <easylogging++.h>

namespace phobos {

// TODO: Another view is needed for laboratory. This should hide some widgets from navigation bar
// Top quality photo should be processed and displayed (maybe from cache)
// Allow displaying original and processed photo
// Allow undo
// Allow at least white balance, hist value equalization, HSV stretch.
// Possibly allow all channels from HSV/RGB to be independently equalized with some offset from sides as in white balance algorithm
// Allow rotating and clipping
// Allow save -> if override this should probably replace image stored in cache as well as metrics
// If possible, display current metrics for processed image in realtime to see improvements made

//TODO: After first load, set focus on first item
ViewStack::ViewStack(pcontainer::Set const& seriesSet,
                     icache::Cache & cache,
                     SharedWidgets const& sharedWidgets) :
  QStackedWidget(), seriesSet(seriesSet), imageCache(cache), sharedWidgets(sharedWidgets)
{
    setupUI();
    connectSignals();

    if (config::qualified("seriesView.num.default", false))
        currentSeriesWidget = numSeriesView;
    else if (config::qualified("seriesView.row.default", false))
        currentSeriesWidget = rowSeriesView;
    else
        currentSeriesWidget = numSeriesView;
}

pcontainer::Series const& ViewStack::findRequestedSeries(boost::optional<QUuid> const& requestedSeries, int const seriesOffset) const
{
  if (requestedSeries)
    return seriesSet.findNonEmptySeries(*requestedSeries, seriesOffset);

  if (currentWidget() == rowSeriesView)
    if (auto const& currentSeries = rowSeriesView->seriesUuid())
      return seriesSet.findNonEmptySeries(*currentSeries, seriesOffset);

  if (currentWidget() == numSeriesView)
    if (auto const& currentSeries = numSeriesView->seriesUuid())
      return seriesSet.findNonEmptySeries(*currentSeries, seriesOffset);

  if (currentWidget() == laboratoryView)
    if (auto const& currentItem = laboratoryView->currentItem())
      return seriesSet.findNonEmptySeries(currentItem->seriesUuid, seriesOffset);

  if (auto focused = utils::focusedPhotoItemWidget())
    return seriesSet.findNonEmptySeries(focused->photoItem().seriesUuid(), seriesOffset);

  if (!seriesSet.empty())
    return seriesSet.front();

  return utils::asserted::always;
}

pcontainer::Item const& ViewStack::findRequestedPhoto(pcontainer::Series const& requestedSeries, int photoOffset)
{
  if (currentWidget() == laboratoryView)
    if (auto const& currentItem = laboratoryView->currentItem())
      // TODO: Add current ord to photoOffset
      ;

  if (photoOffset < 0)
    return *seriesSet.findNonEmptySeries(requestedSeries.uuid(), -1).item(0);

  if (static_cast<unsigned>(photoOffset) >= requestedSeries.size())
    return *seriesSet.findNonEmptySeries(requestedSeries.uuid(), 1).back();

  return *requestedSeries[photoOffset];
}

void ViewStack::welcomeScreenSwitch()
{
  if (seriesSet.hasPhotos())
  {
    if (currentWidget() == welcomeView)
    {
      LOG(INFO) << "Switching to all series view from welcome screen";
      setCurrentWidget(allSeriesView);
    }
  }
  else
  {
    LOG(INFO) << "Switching to welcome screen";
    setCurrentWidget(welcomeView);
  }
}

void ViewStack::setCurrentWidget(QWidget *widget)
{
  sharedWidgets.slider->setVisible(widget == rowSeriesView);
  sharedWidgets.leftRightNav->setVisible(widget == numSeriesView);

  QStackedWidget::setCurrentWidget(widget);
}

void ViewStack::handleSwitchView(ViewDescriptionPtr viewDesc)
{
  if (!seriesSet.hasPhotos())
    return; // return NO-OP

  pcontainer::Series const& targetSeries = findRequestedSeries(viewDesc->seriesUuid, viewDesc->seriesOffset);

  if ((viewDesc->type == ViewType::ALL_SERIES) ||
      ((viewDesc->type == ViewType::CURRENT) && currentWidget() == allSeriesView))
  {
    switchToAllSeries(targetSeries);
    return;
  }

  if ((viewDesc->type == ViewType::LABORATORY) ||
      ((viewDesc->type == ViewType::CURRENT) && currentWidget() == laboratoryView))
  {
    switchToLaboratory(findRequestedPhoto(targetSeries, viewDesc->photoOffset));
    return;
  }

  if (viewDesc->type == ViewType::NUM_SINGLE_SERIES)
  {
    if (currentSeriesWidget != numSeriesView)
    {
      rowSeriesView->clear();
      rowSeriesView->update();
    }
    currentSeriesWidget = numSeriesView;
  }
  else if (viewDesc->type == ViewType::ROW_SINGLE_SERIES)
  {
    if (currentSeriesWidget != rowSeriesView)
    {
      numSeriesView->clear();
      numSeriesView->update();
    }
    currentSeriesWidget = rowSeriesView;
  }

  currentSeriesWidget->showSeries(targetSeries);

  LOG(INFO) << "Switching to " << (currentSeriesWidget == numSeriesView ? "num" : "row") << " series view";
  setCurrentWidget(currentSeriesWidget);
}

void ViewStack::switchToAllSeries(pcontainer::Series const& targetSeries)
{
  LOG(INFO) << "Switching to all series view";
  setCurrentWidget(allSeriesView);
  allSeriesView->focusSeries(targetSeries.uuid());
}

void ViewStack::switchToLaboratory(pcontainer::Item const& item)
{
  LOG(INFO) << "Switching to laboratory view";
  setCurrentWidget(laboratoryView);
  laboratoryView->showItem(item);
}

namespace {
  void selectBestPhotos(pcontainer::Set const& seriesSet, icache::Cache const& cache)
  {
    for (auto const& series : seriesSet)
      for (auto const& item : *series)
      {
        auto const& itemMetrics = cache.metrics().get(item->id());
        if (itemMetrics && itemMetrics->seriesScores && itemMetrics->seriesScores->bestQuality)
        {
          item->select();
          break; // inner loop, continue to next series
        }
      }
  }
  void selectAllPhotos(pcontainer::Set const& seriesSet)
  {
    for (auto const& series : seriesSet)
      for (auto const& item : *series)
        item->select();
  }
  void invertSelections(pcontainer::Set const& seriesSet)
  {
    for (auto const& series : seriesSet)
      for (auto const& item : *series)
        item->invert();
  }
  void clearSelections(pcontainer::Set const& seriesSet)
  {
    for (auto const& series : seriesSet)
      for (auto const& item : *series)
        item->reset();
  }
} // unnamed namespace

void ViewStack::bulkSelect(PhotoBulkAction const action)
{
    switch(action)
    {
    case PhotoBulkAction::SELECT_BEST:
        selectBestPhotos(seriesSet, imageCache);
        break;
    case PhotoBulkAction::SELECT_ALL:
        selectAllPhotos(seriesSet);
        break;
    case PhotoBulkAction::INVERT:
        invertSelections(seriesSet);
        break;
    case PhotoBulkAction::CLEAR:
        clearSelections(seriesSet);
        break;
    }
}

void ViewStack::setupUI()
{
  welcomeView = new WelcomeView();
  allSeriesView = new AllSeriesView(seriesSet, imageCache);
  rowSeriesView = new RowSeriesView(seriesSet, imageCache);
  numSeriesView = new NumSeriesView(seriesSet, imageCache);
  laboratoryView = new LaboratoryView(seriesSet, imageCache);

  addWidget(welcomeView);
  addWidget(allSeriesView);
  addWidget(numSeriesView);
  addWidget(rowSeriesView);
  addWidget(laboratoryView);
}

void ViewStack::connectSignals()
{
  QObject::connect(allSeriesView, &AllSeriesView::switchView, this, &ViewStack::handleSwitchView);
  QObject::connect(rowSeriesView, &RowSeriesView::switchView, this, &ViewStack::handleSwitchView);
  QObject::connect(numSeriesView, &RowSeriesView::switchView, this, &ViewStack::handleSwitchView);

  QObject::connect(sharedWidgets.slider, &widgets::StatusBarSlider::valueChanged, rowSeriesView, &RowSeriesView::resizeImages);

  QObject::connect(sharedWidgets.leftRightNav, &widgets::StatusBarLeftRightNavigation::leftClicked,
                   numSeriesView, &NumSeriesView::showPrevItem);
  QObject::connect(sharedWidgets.leftRightNav, &widgets::StatusBarLeftRightNavigation::rightClicked,
                   numSeriesView, &NumSeriesView::showNextItem);

  QObject::connect(&seriesSet, &pcontainer::Set::newSeries, this, &ViewStack::welcomeScreenSwitch);
  QObject::connect(&seriesSet, &pcontainer::Set::changedSeries, this, &ViewStack::welcomeScreenSwitch);
}

ViewStack::SelectionStatus ViewStack::getSelectionStatus() const
{
  SelectionStatus result;

  for (auto const& series : seriesSet)
  {
    result.status.emplace_back();
    for (auto const& photo : *series)
    {
      switch(photo->state())
      {
      case pcontainer::ItemState::SELECTED:
        result.status.back().selected.push_back(photo->fileName());
        break;
      default:
        result.status.back().others.push_back(photo->fileName());
        break;
      }
    }
  }

  return result;
}

} // namespace phobos
