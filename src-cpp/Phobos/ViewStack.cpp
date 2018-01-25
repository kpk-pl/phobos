#include "ViewStack.h"
#include "WelcomeView.h"
#include "AllSeriesView.h"
#include "NumSeriesView.h"
#include "RowSeriesView.h"
#include "Utils/Focused.h"
#include "ConfigExtension.h"
#include "PhotoContainers/Set.h"
#include "ImageCache/Cache.h"
#include "Utils/Asserted.h"
#include <easylogging++.h>

namespace phobos {

ViewStack::ViewStack(pcontainer::Set const& seriesSet, icache::Cache & cache) :
    QStackedWidget(), seriesSet(seriesSet), imageCache(cache)
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

pcontainer::Series const& ViewStack::findRequestedSeries(ViewDescriptionPtr const& viewDesc) const
{
  if (viewDesc->seriesUuid)
    return seriesSet.findNonEmptySeries(*viewDesc->seriesUuid, viewDesc->seriesOffset.value_or(0));

  auto const focused = utils::focusedPhotoItemWidget();
  if (focused)
    return seriesSet.findNonEmptySeries(focused->photoItem().seriesUuid(), viewDesc->seriesOffset.value_or(0));
  else if (!seriesSet.empty())
    return seriesSet.front();
  else
    return utils::asserted::always;
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

void ViewStack::handleSwitchView(ViewDescriptionPtr viewDesc)
{
  if (!seriesSet.hasPhotos())
    return; // return NO-OP

  pcontainer::Series const& targetSeries = findRequestedSeries(viewDesc);

  if ((viewDesc->type == ViewType::ALL_SERIES) ||
      ((viewDesc->type == ViewType::CURRENT) && currentWidget() == allSeriesView))
  {
    LOG(INFO) << "Switching to all series view";
    setCurrentWidget(allSeriesView);
    allSeriesView->focusSeries(targetSeries.uuid());
    return;
  }

  if (currentSeriesInView == targetSeries.uuid())
  {
    if (viewDesc->type == ViewType::NUM_SINGLE_SERIES && currentSeriesWidget == rowSeriesView)
    {
      rowSeriesView->clear();
      rowSeriesView->update();
      numSeriesView->showSeries(targetSeries);
      currentSeriesWidget = numSeriesView;
    }
    else if (viewDesc->type == ViewType::ROW_SINGLE_SERIES && currentSeriesWidget == numSeriesView)
    {
      numSeriesView->clear();
      numSeriesView->update();
      rowSeriesView->showSeries(targetSeries);
      currentSeriesWidget = rowSeriesView;
    }
  }
  else
  {
    if (viewDesc->type == ViewType::ROW_SINGLE_SERIES)
      currentSeriesWidget = rowSeriesView;
    else if (viewDesc->type == ViewType::NUM_SINGLE_SERIES)
      currentSeriesWidget = numSeriesView;

    currentSeriesInView = targetSeries.uuid();
    currentSeriesWidget->showSeries(targetSeries);
  }

  LOG(INFO) << "Switching to " << (currentSeriesWidget == numSeriesView ? "num" : "row") << " series view";
  setCurrentWidget(currentSeriesWidget);
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
        item->deselect();
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

  addWidget(welcomeView);
  addWidget(allSeriesView);
  addWidget(numSeriesView);
  addWidget(rowSeriesView);
}

void ViewStack::connectSignals()
{
  QObject::connect(allSeriesView, &AllSeriesView::switchView, this, &ViewStack::handleSwitchView);
  QObject::connect(rowSeriesView, &RowSeriesView::switchView, this, &ViewStack::handleSwitchView);
  QObject::connect(numSeriesView, &RowSeriesView::switchView, this, &ViewStack::handleSwitchView);

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
