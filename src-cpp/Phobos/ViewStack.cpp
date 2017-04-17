#include "ViewStack.h"
#include "AllSeriesView.h"
#include "NumSeriesView.h"
#include "RowSeriesView.h"
#include "Utils/Focused.h"
#include "ConfigExtension.h"

namespace phobos {

ViewStack::ViewStack() :
    QStackedWidget()
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

void ViewStack::addPhotos(QStringList const& photos)
{
    seriesSet.addPhotos(photos);
}

pcontainer::SeriesPtr const& ViewStack::findRequestedSeries(ViewDescriptionPtr const& viewDesc) const
{
    if (!viewDesc->seriesUuid)
    {
        auto const focused = utils::focusedPhotoItemWidget();
        if (focused)
            return seriesSet.findSeries(focused->photoItem().seriesUuid(), viewDesc->seriesOffset.value_or(0));
        else if (!seriesSet.empty())
            return seriesSet.front();
        else
            assert(false);
    }

    return seriesSet.findSeries(*viewDesc->seriesUuid, viewDesc->seriesOffset.value_or(0));
}

void ViewStack::updateCurrentWidget(ViewDescriptionPtr const& viewDesc)
{
    switch(viewDesc->type)
    {
    case ViewType::ROW_SINGLE_SERIES:
        currentSeriesWidget = rowSeriesView;
        break;
    case ViewType::NUM_SINGLE_SERIES:
        currentSeriesWidget = numSeriesView;
        break;
    case ViewType::ANY_SINGLE_SERIES:
    case ViewType::ALL_SERIES:
    case ViewType::CURRENT:
        break; // no action needed
    default:
        assert(false);
    }
}

void ViewStack::handleSwitchView(ViewDescriptionPtr viewDesc)
{
    if (seriesSet.empty())
        return; // NO-OP

    pcontainer::SeriesPtr const& targetSeries = findRequestedSeries(viewDesc);
    updateCurrentWidget(viewDesc);

    if ((viewDesc->type == ViewType::ALL_SERIES) ||
        ((viewDesc->type == ViewType::CURRENT) && currentWidget() == allSeriesView))
    {
        setCurrentWidget(allSeriesView);
        allSeriesView->focusSeries(targetSeries->uuid());
        return;
    }

    if (currentSeriesInView == targetSeries->uuid())
    {
        if (viewDesc->type == ViewType::NUM_SINGLE_SERIES && currentWidget() == rowSeriesView)
        {
            numSeriesView->exchangeItemsFrom(rowSeriesView);
        }
        else if (viewDesc->type == ViewType::ROW_SINGLE_SERIES && currentWidget() == numSeriesView)
        {
            rowSeriesView->exchangeItemsFrom(numSeriesView);
        }
    }
    else {
        currentSeriesInView = targetSeries->uuid();
        currentSeriesWidget->showSeries(targetSeries);
    }

    setCurrentWidget(currentSeriesWidget);
}

namespace {
    void selectBestPhotos(pcontainer::Set const& seriesSet)
    {
        for (auto const& series : seriesSet)
        {
            pcontainer::ItemPtr bestItem = series->best();
            if (bestItem)
               bestItem->select();
        }
    }
    void selectUncheckedPhotos(pcontainer::Set const& seriesSet)
    {
        for (auto const& series : seriesSet)
        {
            for (auto const& item : *series)
                if (item->state() == pcontainer::ItemState::UNKNOWN)
                    item->select();
        }
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
        selectBestPhotos(seriesSet);
        break;
    case PhotoBulkAction::SELECT_UNCHECKED:
        selectUncheckedPhotos(seriesSet);
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
    allSeriesView = new AllSeriesView();
    rowSeriesView = new RowSeriesView();
    numSeriesView = new NumSeriesView();

    addWidget(allSeriesView);
    addWidget(numSeriesView);
    addWidget(rowSeriesView);
}

void ViewStack::connectSignals()
{
    QObject::connect(&seriesSet, &pcontainer::Set::newSeries, allSeriesView, &AllSeriesView::addNewSeries);
    QObject::connect(allSeriesView, &AllSeriesView::switchView, this, &ViewStack::handleSwitchView);

    QObject::connect(rowSeriesView, &RowSeriesView::switchView, this, &ViewStack::handleSwitchView);
    QObject::connect(numSeriesView, &RowSeriesView::switchView, this, &ViewStack::handleSwitchView);
}

} // namespace phobos
