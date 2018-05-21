#include "ViewStack.h"
#include "MainToolbar.h"
#include "Views/Welcome.h"
#include "Views/AllSeries.h"
#include "Views/NumSeries.h"
#include "Views/RowSeries.h"
#include "Views/Laboratory.h"
#include "Utils/Focused.h"
#include "ConfigExtension.h"
#include "PhotoContainers/Set.h"
#include "ImageCache/Cache.h"
#include "Widgets/StatusBarSlider.h"
#include "Widgets/StatusBarLeftRightNavigation.h"
#include "Widgets/ImageShowDialog.h"
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
ViewStack::ViewStack(pcontainer::Set const& seriesSet,
                     icache::Cache & cache,
                     SharedWidgets const& sharedWidgets,
                     MainToolbar *mainToolbar) :
  QStackedWidget(), seriesSet(seriesSet), imageCache(cache), sharedWidgets(sharedWidgets), mainToolbar(mainToolbar)
{
  setupUI();
  connectSignals();

  if (config::qualified("seriesView.num.default", false))
    currentSeriesWidget = numSeriesView;
  else if (config::qualified("seriesView.row.default", false))
    currentSeriesWidget = rowSeriesView;
  else
    currentSeriesWidget = numSeriesView;

  setCurrentWidget(welcomeView);
}

pcontainer::Series const& ViewStack::findRequestedSeries(ViewDescriptionPtr const& viewDesc) const
{
  if (viewDesc->seriesUuid)
    return seriesSet.findNonEmptySeries(*viewDesc->seriesUuid, viewDesc->seriesOffset);

  if (currentWidget() == rowSeriesView)
    if (auto const& currentSeries = rowSeriesView->seriesUuid())
      return seriesSet.findNonEmptySeries(*currentSeries, viewDesc->seriesOffset);

  if (currentWidget() == numSeriesView)
    if (auto const& currentSeries = numSeriesView->seriesUuid())
      return seriesSet.findNonEmptySeries(*currentSeries, viewDesc->seriesOffset);

  if (currentWidget() == laboratoryView)
    if (auto const& currentItem = laboratoryView->currentItem())
      return seriesSet.findNonEmptySeries(currentItem->seriesUuid, viewDesc->seriesOffset);

  if (auto focused = utils::focusedPhotoItemWidget())
  {
    viewDesc->photoOffset += focused->photoItem().ord();
    return seriesSet.findNonEmptySeries(focused->photoItem().seriesUuid(), viewDesc->seriesOffset);
  }

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
      setCurrentWidget(allSeriesView);
  }
  else
  {
    setCurrentWidget(welcomeView);
  }
}

namespace {
void setToolbarVisibility(MainToolbar * toolbar, config::ConfigPath const& configPath)
{
  auto const supported = config::qualified(configPath("toolbarSupport"), std::set<std::string>{});
  for (auto const& group : toolbar->buttonGroups())
    toolbar->setGroupVisible(group, supported.find(group) != supported.end());
}
} // unnamed namespace

void ViewStack::setCurrentWidget(QWidget *widget)
{
  sharedWidgets.slider->setVisible(widget == rowSeriesView);
  sharedWidgets.leftRightNav->setVisible(widget == numSeriesView);

  auto const tryWidget = [&](QWidget *wgt, char const* name){
    if (widget != wgt)
      return false;

    LOG(TRACE) << "Switching to " << name;
    setToolbarVisibility(mainToolbar, config::ConfigPath(name));
    return true;
  };

  tryWidget(welcomeView, "welcomeView") ||
  tryWidget(allSeriesView, "allSeriesView") ||
  tryWidget(rowSeriesView, "seriesView.row") ||
  tryWidget(numSeriesView, "seriesView.num") ||
  tryWidget(laboratoryView, "laboratoryView");

  QStackedWidget::setCurrentWidget(widget);
}

void ViewStack::handleSwitchView(ViewDescriptionPtr viewDesc)
{
  if (!seriesSet.hasPhotos())
    return; // return NO-OP

  pcontainer::Series const& targetSeries = findRequestedSeries(viewDesc);

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
  LOG(TRACE) << "Current series in view: " << targetSeries.uuid().toString();
  setCurrentWidget(currentSeriesWidget);
}

void ViewStack::switchToAllSeries(pcontainer::Series const& targetSeries)
{
  setCurrentWidget(allSeriesView);
  allSeriesView->focusSeries(targetSeries.uuid());
}

void ViewStack::switchToLaboratory(pcontainer::Item const& item)
{
  setCurrentWidget(laboratoryView);
  laboratoryView->changePhoto(item);
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
  welcomeView = new view::Welcome();
  allSeriesView = new view::AllSeries(seriesSet, imageCache);
  rowSeriesView = new view::RowSeries(seriesSet, imageCache);
  numSeriesView = new view::NumSeries(seriesSet, imageCache);
  laboratoryView = new view::Laboratory(seriesSet, imageCache);

  addWidget(welcomeView);
  addWidget(allSeriesView);
  addWidget(numSeriesView);
  addWidget(rowSeriesView);
  addWidget(laboratoryView);
}

void ViewStack::connectSignals()
{
  QObject::connect(allSeriesView, &view::AllSeries::switchView, this, &ViewStack::handleSwitchView);
  QObject::connect(rowSeriesView, &view::RowSeries::switchView, this, &ViewStack::handleSwitchView);
  QObject::connect(numSeriesView, &view::NumSeries::switchView, this, &ViewStack::handleSwitchView);

  QObject::connect(allSeriesView, &view::AllSeries::showImageFullscreen, this, &ViewStack::showImageFullscreen);
  QObject::connect(rowSeriesView, &view::RowSeries::showImageFullscreen, this, &ViewStack::showImageFullscreen);
  QObject::connect(numSeriesView, &view::NumSeries::showImageFullscreen, this, &ViewStack::showImageFullscreen);

  QObject::connect(sharedWidgets.slider, &widgets::StatusBarSlider::valueChanged, rowSeriesView, &view::RowSeries::resizeImages);

  QObject::connect(sharedWidgets.leftRightNav, &widgets::StatusBarLeftRightNavigation::leftClicked,
                   numSeriesView, &view::NumSeries::showPrevItem);
  QObject::connect(sharedWidgets.leftRightNav, &widgets::StatusBarLeftRightNavigation::rightClicked,
                   numSeriesView, &view::NumSeries::showNextItem);

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

void ViewStack::photoEnhancement(iprocess::enhance::OperationType const operation)
{
  if (currentWidget() != laboratoryView)
  {
    LOG(TRACE) << "Ignoring photo enhancement operation because not in laboratory view";
    return;
  }

  laboratoryView->process(operation);
}

boost::optional<pcontainer::ItemId> ViewStack::currentItemInLaboratory() const
{
  if (currentWidget() != laboratoryView)
    return boost::none;

  return laboratoryView->currentItem();
}

void ViewStack::saveItemInLaboratory(QString const& fileName) const
{
  if (currentWidget() != laboratoryView)
    return;

  laboratoryView->saveItem(fileName);
}

// TODO: FullScreen must use proactive approach. But also needs to be persistent.
// Priorities in cache does not work very well with this combination
// Cache cannot use priority 0 for all persistent loads.
// Maybe another flag in cache?
void ViewStack::showImageFullscreen(pcontainer::ItemId const& itemId)
{
  auto earlyResult = imageCache.transaction().item(itemId).callback([itemId](auto const& result){
      widgets::fulldialog::updateImage(result.image, itemId);
  }).persistent().execute();

  widgets::fulldialog::showImage(window(), earlyResult.image, itemId);
}

} // namespace phobos
