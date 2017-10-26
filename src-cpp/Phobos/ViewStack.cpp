#include "ViewStack.h"
#include "AllSeriesView.h"
#include "NumSeriesView.h"
#include "RowSeriesView.h"
#include "Utils/Focused.h"
#include "ConfigExtension.h"
#include "PhotoContainers/Set.h"
#include "ImageCache/Cache.h"
#include "Utils/Asserted.h"

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

void ViewStack::handleSwitchView(ViewDescriptionPtr viewDesc)
{
  if (!seriesSet.hasPhotos())
    return; // NO-OP

  pcontainer::Series const& targetSeries = findRequestedSeries(viewDesc);

  if ((viewDesc->type == ViewType::ALL_SERIES) ||
      ((viewDesc->type == ViewType::CURRENT) && currentWidget() == allSeriesView))
  {
    setCurrentWidget(allSeriesView);
    allSeriesView->focusSeries(targetSeries.uuid());
    return;
  }

  if (currentSeriesInView == targetSeries.uuid())
  {
    if (viewDesc->type == ViewType::NUM_SINGLE_SERIES && currentSeriesWidget == rowSeriesView)
    {
      rowSeriesView->clear();
      numSeriesView->showSeries(targetSeries);
      currentSeriesWidget = numSeriesView;
    }
    else if (viewDesc->type == ViewType::ROW_SINGLE_SERIES && currentSeriesWidget == numSeriesView)
    {
      numSeriesView->clear();
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

  setCurrentWidget(currentSeriesWidget);
}

namespace {
  void selectBestPhotos(pcontainer::Set const& seriesSet, icache::Cache const& cache)
  {
    for (auto const& series : seriesSet)
      for (auto const& item : *series)
      {
        auto const& itemMetrics = cache.metrics().get(item->id());
        if (itemMetrics && itemMetrics->bestQuality)
        {
          item->select();
          break; // inner loop, continue to next series
        }
      }
  }
  void selectUncheckedPhotos(pcontainer::Set const& seriesSet)
  {
    for (auto const& series : seriesSet)
      for (auto const& item : *series)
        if (item->state() == pcontainer::ItemState::UNKNOWN)
          item->select();
  }
  void discardUncheckedPhotos(pcontainer::Set const& seriesSet)
  {
    for (auto const& series : seriesSet)
      for (auto const& item : *series)
        if (item->state() == pcontainer::ItemState::UNKNOWN)
          item->discard();
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
    case PhotoBulkAction::SELECT_UNCHECKED:
        selectUncheckedPhotos(seriesSet);
        break;
    case PhotoBulkAction::DISCARD_UNCHECKED:
        discardUncheckedPhotos(seriesSet);
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
//  setStyleSheet("background-color: red");
    allSeriesView = new AllSeriesView(seriesSet, imageCache);
    rowSeriesView = new RowSeriesView(seriesSet, imageCache);
    numSeriesView = new NumSeriesView(seriesSet, imageCache);

    addWidget(allSeriesView);
    addWidget(numSeriesView);
    addWidget(rowSeriesView);
}

void ViewStack::connectSignals()
{
    QObject::connect(allSeriesView, &AllSeriesView::switchView, this, &ViewStack::handleSwitchView);
    QObject::connect(rowSeriesView, &RowSeriesView::switchView, this, &ViewStack::handleSwitchView);
    QObject::connect(numSeriesView, &RowSeriesView::switchView, this, &ViewStack::handleSwitchView);
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
            case pcontainer::ItemState::DISCARDED:
                result.status.back().discarded.push_back(photo->fileName());
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
