#include <functional>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QEvent>
#include <QKeyEvent>
#include <QPixmap>
#include "AllSeriesView.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/Addon.h"
#include "Widgets/PhotoItem/Capability.h"
#include "Utils/Algorithm.h"
#include "Utils/Focused.h"
#include "Utils/Asserted.h"
#include "Utils/LayoutClear.h"
#include "ImageCache/Cache.h"
#include <easylogging++.h>

namespace phobos {

// TODO: Rightclick menu option to remove series entirely
// think how this can be done, maybe removing objects from grid is enough to leave empty row ?
// But this will mess up with numbering
//
// TODO: Add series numbering
// left to each photo should be a number, colorcode series that have at least one photo chosen

namespace {
class ArrowFilter : public QObject
{
public:
    ArrowFilter(QObject* parent = nullptr) :
        QObject(parent)
    {}

    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
            auto const& key = keyEvent->key();
            if (key == Qt::Key_Down || key == Qt::Key_Up ||
                key == Qt::Key_Left || key == Qt::Key_Right)
            {
                event->ignore();
                return true;
            }
        }
        return QObject::eventFilter(watched, event);
    }
};
} // unnamed namespace

struct AllSeriesView::Coords
{
    template<typename T, typename U>
    Coords(T row, U col) :
        row(int(row)), col(int(col))
    {}
    static const int MAX = std::numeric_limits<int>::max();
    int row, col;
};

AllSeriesView::AllSeriesView(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
    seriesSet(seriesSet), imageCache(imageCache)
{
    QObject::connect(&imageCache, &icache::Cache::updateImage, this, &AllSeriesView::updateImage);
    QObject::connect(&imageCache, &icache::Cache::updateMetrics, this, &AllSeriesView::updateMetrics);
    QObject::connect(&seriesSet, &pcontainer::Set::newSeries, this, &AllSeriesView::addNewSeries);
    QObject::connect(&seriesSet, &pcontainer::Set::changedSeries, this, &AllSeriesView::updateExistingSeries);

    // TODO: navigationBar

    grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(config::qualified("allSeriesView.photosSpacing", 3u));
    grid->setVerticalSpacing(config::qualified("allSeriesView.seriesSpacing", 15u));

    QVBoxLayout* scrollLayout = new QVBoxLayout();
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->addLayout(grid);
    scrollLayout->addStretch();

    QWidget* scrollWidget = new QWidget();
    scrollWidget->setLayout(scrollLayout);

    QScrollArea* scroll = new QScrollArea();
    scroll->installEventFilter(new ArrowFilter(scroll));
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(scrollWidget);

    QVBoxLayout* vlayout = new QVBoxLayout();
    // TODO: vlayout->addWidget(navigationBar);
    vlayout->addWidget(scroll);

    setLayout(vlayout);
}

void AllSeriesView::focusSeries()
{
  if (numberOfSeries() > 0)
    grid->itemAtPosition(0, 0)->widget()->setFocus();
}

void AllSeriesView::focusSeries(QUuid const seriesUuid)
{
  assert(utils::valueIn(seriesUuid, seriesUuidToRow));
  grid->itemAtPosition(seriesUuidToRow[seriesUuid], 0)->widget()->setFocus();
}

void AllSeriesView::addNewSeries(pcontainer::SeriesPtr series)
{
  std::size_t const row = numberOfSeries();
  seriesUuidToRow.emplace(series->uuid(), row);

  for (std::size_t col = 0; col < series->size(); ++col)
  {
    pcontainer::ItemPtr const photo = series->item(col);
    addItemToGrid(row, col, photo);
  }
}

namespace {
  using PhotoItemsContentMap = std::map<pcontainer::ItemId, std::unique_ptr<widgets::pitem::PhotoItem> >;

  PhotoItemsContentMap clearRowInGrid(QGridLayout *grid, std::size_t const row)
  {
    LOG(DEBUG) << "Clearing row " << row;
    PhotoItemsContentMap oldContent;

    for (int idx = 0; idx < grid->count(); /* none */)
    {
      int r, c, rSpan, cSpan;
      grid->getItemPosition(idx, &r, &c, &rSpan, &cSpan);
      if (static_cast<long>(row) != r)
      {
        ++idx;
        continue;
      }

      QLayoutItem *layoutItem = grid->takeAt(idx);
      assert(layoutItem);

      std::unique_ptr<widgets::pitem::PhotoItem> photoItem(dynamic_cast<widgets::pitem::PhotoItem*>(layoutItem->widget()));
      if (!photoItem)
      {
        utils::clearLayoutItem(layoutItem);
        continue;
      }

      LOG(DEBUG) << "Saving item at column " << c << ": " << photoItem->photoItem().id().toString();
      oldContent.emplace(photoItem->photoItem().id(), std::move(photoItem));
      utils::clearLayoutItem(layoutItem, false);
    }

    LOG(DEBUG) << "Returned " << oldContent.size() << " items";
    return oldContent;
  }
} // unnamed namespace

void AllSeriesView::updateExistingSeries(QUuid seriesUuid)
{
  auto const seriesRow = utils::asserted::fromMap(seriesUuidToRow, seriesUuid);
  auto oldContent = clearRowInGrid(grid, seriesRow);

  pcontainer::SeriesPtr const& series = seriesSet.findSeries(seriesUuid);
  assert(series);

  for (std::size_t col = 0; col < series->size(); ++col)
  {
    pcontainer::ItemPtr const item = series->item(col);
    auto const it = oldContent.find(item->id());
    if (it == oldContent.end() || !it->second)
    {
      LOG(DEBUG) << "Adding at col " << col << " newly constructed item " << item->id().toString();
      addItemToGrid(seriesRow, col, item);
    }
    else
    {
      LOG(DEBUG) << "Adding at col " << col << " item from saved content " << it->second->photoItem().id().toString();
      grid->addWidget(it->second.release(), seriesRow, col);
    }
  }

  LOG(DEBUG) << std::count_if(oldContent.begin(), oldContent.end(), [](auto const& p){return p.second != nullptr;})
             << " items were left from saved content";
}

void AllSeriesView::addItemToGrid(int row, int col, pcontainer::ItemPtr const& itemPtr)
{
  using namespace widgets::pitem;

  auto const widgetAddons = Addons(config::get()->get_qualified_array_of<std::string>("allSeriesView.enabledAddons").value_or({}));

  auto const& itemId = itemPtr->id();
  QImage const thumbnail = imageCache.execute(imageCache.transaction().item(itemId).thumbnail());
  PhotoItem* item = new PhotoItem(itemPtr, thumbnail, widgetAddons, CapabilityType::OPEN_SERIES | CapabilityType::REMOVE_PHOTO);
  item->setMetrics(imageCache.metrics().get(itemId));

  QObject::connect(item, &PhotoItem::openInSeries,
    [this](QUuid const& uuid){ switchView(ViewDescription::make(ViewType::ANY_SINGLE_SERIES, uuid)); });

  QObject::connect(item, &PhotoItem::changeSeriesState, this, &AllSeriesView::changeSeriesState);
  QObject::connect(item, &PhotoItem::removeFromSeries, &seriesSet, &pcontainer::Set::removeImage);

  grid->addWidget(item, row, col);
}

void AllSeriesView::updateImage(pcontainer::ItemId const& itemId)
{
  auto& widget = utils::asserted::fromPtr(findItem(itemId));
  widget.setImage(imageCache.execute(imageCache.transaction().item(itemId).thumbnail()));
}

void AllSeriesView::updateMetrics(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics)
{
  auto& widget = utils::asserted::fromPtr(findItem(itemId));
  widget.setMetrics(metrics);
}

// TODO: lookup with itemAtPosition is probably not that good
widgets::pitem::PhotoItem* AllSeriesView::findItem(pcontainer::ItemId const& itemId) const
{
    auto const seriesRow = utils::asserted::fromMap(seriesUuidToRow, itemId.seriesUuid);

    for (int i = 0; i < grid->columnCount(); ++i)
    {
        auto const& widgetItem = grid->itemAtPosition(seriesRow, i);
        if (!widgetItem || !widgetItem->widget())
            continue;

        auto const photoItemWgt = dynamic_cast<widgets::pitem::PhotoItem*>(widgetItem->widget());
        assert(photoItemWgt);

        if (photoItemWgt->photoItem().id() == itemId)
          return photoItemWgt;
    }

    assert(false); // impossible if image is not found
    return nullptr;
}

void AllSeriesView::keyPressEvent(QKeyEvent* keyEvent)
{
    if (keyEvent->type() == QEvent::KeyPress &&
            utils::valueIn(keyEvent->key(), {Qt::Key_Left, Qt::Key_Right, Qt::Key_Up, Qt::Key_Down}))
    {
        auto const focusCoords = focusGridCoords();
        if (focusCoords)
        {
            Coords const jump = findValidProposal(nextJumpProposals(*focusCoords, keyEvent->key()));
            grid->itemAtPosition(jump.row, jump.col)->widget()->setFocus();
        }
        else if (grid->count() > 0)
            grid->itemAtPosition(0, 0)->widget()->setFocus();
    }

    QWidget::keyPressEvent(keyEvent);
}

boost::optional<AllSeriesView::Coords> AllSeriesView::focusGridCoords() const
{
    widgets::pitem::PhotoItem* focusItem = utils::focusedPhotoItemWidget();
    if (!focusItem)
        return boost::none;

    QUuid const& focusSeries = focusItem->photoItem().seriesUuid();
    assert(utils::valueIn(focusSeries, seriesUuidToRow));

    unsigned const focusRow = seriesUuidToRow.find(focusSeries)->second;
    for (int i = 0; i < grid->columnCount(); ++i)
    {
        auto const& widgetItem = grid->itemAtPosition(focusRow, i);
        if (!widgetItem || !widgetItem->widget())
            continue;
        if (widgetItem->widget() == focusItem)
            return Coords{focusRow, i};
    }

    assert(false); // should ALWAYS find item in the loop
    return boost::none;
}

std::vector<AllSeriesView::Coords>
    AllSeriesView::nextJumpProposals(Coords const& coords, int const directionKey) const
{
    unsigned const row = coords.row;
    unsigned const col = coords.col;
    unsigned const maxRow = grid->rowCount()-1;

    switch(directionKey)
    {
    case Qt::Key_Right:
        return {{row, col+1}, {row+1, 0}, {0, 0}};
    case Qt::Key_Down:
        return {{row+1, col}, {row+1, Coords::MAX}, {0, col}, {0, Coords::MAX}};
    case Qt::Key_Left:
        return {{row, col-1}, {row-1, Coords::MAX}, {maxRow, Coords::MAX}};
    case Qt::Key_Up:
        return {{row-1, col}, {row-1, Coords::MAX}, {maxRow, col}, {maxRow, Coords::MAX}};
    }

    assert(false);
    return {};
}

AllSeriesView::Coords AllSeriesView::findValidProposal(std::vector<Coords> const& proposals) const
{
    assert(grid->count() > 0); // Should never be called on empty grid

    for (Coords const& c : proposals)
    {
        if (c.row < 0 || c.row >= grid->rowCount())
            continue;
        if (c.col < 0 || (c.col >= grid->columnCount() && c.col != Coords::MAX))
            continue;
        if (c.col == Coords::MAX)
        {
            for (int i = 0; i <= grid->columnCount(); ++i)
                if (!grid->itemAtPosition(c.row, i))
                    if (i != 0)
                        return {c.row, i-1};
                    break;
        }
        else
        {
            if (grid->itemAtPosition(c.row, c.col))
                return {c.row, c.col};
        }
    }

    /*
     * Should never happen if proposals are correctly constructed.
     * On each proposal list should be at least one (0,0) or (0, None). When grid has at least one
     * correctly positioned element at (0,0) it should be found.
     */
    assert(false); // No valid proposals found
    return {0, 0};
}

// TODO: Presumably using itemAtPosition is not that quick especially when done in loops
// Try to iterate to grid->count(), use getItemPosition to get row and column
void AllSeriesView::changeSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state)
{
    assert(utils::valueIn(seriesUuid, seriesUuidToRow));
    unsigned const seriesRow = seriesUuidToRow.find(seriesUuid)->second;

    for (int i = 0; i < grid->columnCount(); ++i)
    {
        auto const& widgetItem = grid->itemAtPosition(seriesRow, i);
        if (!widgetItem || !widgetItem->widget())
            continue;

        widgets::pitem::PhotoItem* photoWidget = dynamic_cast<widgets::pitem::PhotoItem*>(widgetItem->widget());
        assert(photoWidget);

        photoWidget->photoItem().setState(state);
    }
}

} // namespace phobos
