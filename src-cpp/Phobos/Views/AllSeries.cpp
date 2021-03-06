#include "Views/AllSeries.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/Addon.h"
#include "Widgets/PhotoItem/Capability.h"
#include "Widgets/PhotoItem/Recovery.h"
#include "Utils/Algorithm.h"
#include "Utils/Focused.h"
#include "Utils/Asserted.h"
#include "ImageCache/Cache.h"
#include <easylogging++.h>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <QPushButton>

namespace phobos { namespace view {

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

struct AllSeries::Coords
{
    template<typename T, typename U>
    Coords(T row, U col) :
        row(int(row)), col(int(col))
    {}
    static const int MAX = std::numeric_limits<int>::max();
    int row, col;
};

// TODO: Might distinguish between series and non-series display, maybe by changing font color on the ORD column
AllSeries::AllSeries(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
  View(seriesSet, imageCache), scroll(nullptr), grid(nullptr)
{
  QObject::connect(&imageCache, &icache::Cache::updateMetrics, this, &AllSeries::updateMetrics);
  QObject::connect(&seriesSet, &pcontainer::Set::newSeries, this, &AllSeries::addNewSeries);
  QObject::connect(&seriesSet, &pcontainer::Set::changedSeries, this, &AllSeries::updateExistingSeries);

  prepareUI();
}

void AllSeries::prepareUI()
{
  grid = new QGridLayout();
  grid->setContentsMargins(0, 0, 0, 0);
  grid->setHorizontalSpacing(config::qualified("allSeriesView.photosSpacing", 3u));
  grid->setVerticalSpacing(config::qualified("allSeriesView.seriesSpacing", 15u));

  QVBoxLayout* scrollLayout = new QVBoxLayout();
  scrollLayout->setContentsMargins(0, 0, 0, 0);
  scrollLayout->addLayout(grid);
  scrollLayout->addStretch(1);

  QWidget* scrollWidget = new QWidget();
  scrollWidget->setLayout(scrollLayout);

  scroll = new QScrollArea();
  scroll->installEventFilter(new ArrowFilter(scroll));
  scroll->setWidgetResizable(true);
  scroll->setFrameShape(QFrame::NoFrame);
  scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scroll->setWidget(scrollWidget);

  // TODO: Add some sort of stretch so when each series has one photo and maxHeightLimit takes precedense,
  // photos are aligned to left. The obvious solution to add stretch right to grid layout does not work
  // since stretch has equal precedense to grid and photos are rendered either size 0 or half available space

  QVBoxLayout* newLayout = new QVBoxLayout();
  newLayout->addWidget(scroll);

  setLayout(newLayout);
}

std::size_t AllSeries::maxNumberOfPhotosInRow() const
{
  int const cc = grid->columnCount();
  return cc > 0 ? cc-1 : cc;
}

QWidget* AllSeries::photoInGridAt(std::size_t const row, std::size_t const col) const
{
  QLayoutItem* wItem = grid->itemAtPosition(row, col+1);
  return wItem ? wItem->widget() : nullptr;
}

void AllSeries::addPhotoToGridAt(QWidget *widget, std::size_t const row, std::size_t const col)
{
  grid->addWidget(widget, row, col+1);
}

void AllSeries::addNumberingToGrid(int const number)
{
  QLabel *label = new QLabel(QString("%1").arg(number+1));
  label->setContentsMargins(0, 0, 2, 0);

  auto font = label->font();
  font.setBold(true);
  label->setFont(font);

  grid->addWidget(label, number, 0, Qt::AlignRight);
}

void AllSeries::focusSeries()
{
  setFocusedWidget(photoInGridAt(seriesSet.nonEmpty(0, 0).ord(), 0));
}

void AllSeries::focusSeries(QUuid const seriesUuid)
{
  setFocusedWidget(photoInGridAt(utils::asserted::fromMap(seriesUuidToRow, seriesUuid), 0));
}

void AllSeries::addNewSeries(pcontainer::SeriesPtr series)
{
  bool const firstOne = seriesUuidToRow.empty();
  seriesUuidToRow.emplace(series->uuid(), series->ord());

  if (series->empty())
    return;

  addNumberingToGrid(static_cast<int>(series->ord()));

  for (std::size_t col = 0; col < series->size(); ++col)
  {
    pcontainer::ItemPtr const photo = series->item(col);
    addItemToGrid(series->ord(), col, photo);
  }

  if (firstOne)
    setFocusedWidget(photoInGridAt(series->ord(), 0));
}

namespace {
  widgets::pitem::utils::PhotoItemsContentMap clearRowInGrid(QGridLayout *grid, std::size_t const row)
  {
    LOG(DEBUG) << "Clearing row " << row;

    auto const myRow = [&](int const idx){
      int r, c, rSpan, cSpan;
      grid->getItemPosition(idx, &r, &c, &rSpan, &cSpan);
      return static_cast<long>(row) == r;
    };

    auto oldContent = widgets::pitem::utils::recoverFromLayout(grid, myRow);

    LOG(DEBUG) << "Returned " << oldContent.size() << " old items";
    return oldContent;
  }
} // unnamed namespace

void AllSeries::updateExistingSeries(QUuid seriesUuid)
{
  auto const seriesRow = utils::asserted::fromMap(seriesUuidToRow, seriesUuid);
  auto oldContent = clearRowInGrid(grid, seriesRow);

  pcontainer::Series const& series = seriesSet.findSeries(seriesUuid);

  if (!series.empty())
    addNumberingToGrid(series.ord());

  for (std::size_t col = 0; col < series.size(); ++col)
  {
    pcontainer::ItemPtr const item = series.item(col);
    auto const it = oldContent.find(item->id());
    if (it == oldContent.end() || !it->second)
    {
      LOG(DEBUG) << "Adding at col " << col << " newly constructed item " << item->id().toString();
      addItemToGrid(seriesRow, col, item);
    }
    else
    {
      LOG(DEBUG) << "Adding at col " << col << " item from saved content " << it->second->photoItem().id().toString();
      addPhotoToGridAt(it->second.release(), seriesRow, col);
    }
  }

  LOG(DEBUG) << std::count_if(oldContent.begin(), oldContent.end(), [](auto const& p){return p.second != nullptr;})
             << " items were left from saved content";
}

void AllSeries::addItemToGrid(int const row, int const col, pcontainer::ItemPtr const& itemPtr)
{
  using namespace widgets::pitem;
  static auto const capabilities = CapabilityType::OPEN_SERIES | CapabilityType::REMOVE_PHOTO | CapabilityType::REMOVE_SERIES;

  auto const widgetAddons = Addons(config::qualified("allSeriesView.enabledAddons", std::vector<std::string>{}));
  auto const& itemId = itemPtr->id();

  auto item = std::make_unique<PhotoItem>(itemPtr, widgetAddons, capabilities);

  auto const thumbnail = imageCache.transaction().item(itemId).callback([lt=item->lifetime()](auto && result){
      auto item = lt.lock();
      if (item) item->setImage(result.image);
    }).thumbnail().persistent().execute();

  item->setBorder(config::qualified("photoItemWidget.border.width", 0));

  if (auto h = config::qualified<std::size_t>("allSeriesView.maxPhotoHeight"))
    item->setMaximumHeight(*h);
  item->setImage(thumbnail.image);
  item->setMetrics(imageCache.metrics().get(itemId));

  QObject::connect(item.get(), &PhotoItem::openInSeries, [id = itemPtr->id(), ord = itemPtr->ord(), this](){
    switchView(ViewDescription::switchTo(ViewType::ANY_SINGLE_SERIES, id.seriesUuid, 0, ord));
  });

  QObject::connect(item.get(), &PhotoItem::changeSeriesState, this, &AllSeries::changeSeriesState);
  QObject::connect(item.get(), &PhotoItem::removeFromSeries, &seriesSet, &pcontainer::Set::removeImage);
  QObject::connect(item.get(), &PhotoItem::removeAllSeries, &seriesSet, &pcontainer::Set::removeSeries);
  QObject::connect(item.get(), &PhotoItem::showFullscreen, this, &AllSeries::showImageFullscreen);

  addPhotoToGridAt(item.release(), row, col);
}

void AllSeries::updateMetrics(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics)
{
  auto& widget = utils::asserted::fromPtr(findItem(itemId));
  widget.setMetrics(metrics);
}

widgets::pitem::PhotoItem* AllSeries::findItem(pcontainer::ItemId const& itemId) const
{
  auto const seriesRow = utils::asserted::fromMap(seriesUuidToRow, itemId.seriesUuid);

  for (int idx = 0; idx < grid->count(); ++idx)
  {
    int r, c, rSpan, cSpan;
    grid->getItemPosition(idx, &r, &c, &rSpan, &cSpan);
    if (r != static_cast<int>(seriesRow))
      continue;

    QLayoutItem *lwgt = grid->itemAt(idx);
    QWidget *widgetItem = lwgt ? lwgt->widget() : nullptr;

    auto const photoItemWgt = dynamic_cast<widgets::pitem::PhotoItem*>(widgetItem);
    if (!photoItemWgt)
      continue;

    if (photoItemWgt->photoItem().id() == itemId)
      return photoItemWgt;
  }

  return utils::asserted::always;
}

void AllSeries::keyPressEvent(QKeyEvent* keyEvent)
{
  if (keyEvent->type() == QEvent::KeyPress &&
        utils::valueIn(keyEvent->key(), {Qt::Key_Left, Qt::Key_Right, Qt::Key_Up, Qt::Key_Down}))
  {
    if (auto const focusCoords = focusGridCoords())
    {
      Coords const jump = findValidProposal(nextJumpProposals(*focusCoords, keyEvent->key()));
      LOG(TRACE) << "Key press focusing (" << jump.row << ", " << jump.col << ") from ("
                << focusCoords->row << ", " << focusCoords->col << ")";
      setFocusedWidget(photoInGridAt(jump.row, jump.col));
    }
    else
    {
      LOG(TRACE) << "Key press focusing first photo of first series by default";
      focusSeries();
    }
  }

  QWidget::keyPressEvent(keyEvent);
}

boost::optional<AllSeries::Coords> AllSeries::focusGridCoords() const
{
  widgets::pitem::PhotoItem* focusItem = utils::focusedPhotoItemWidget();
  if (!focusItem)
    return boost::none;

  QUuid const& focusSeries = focusItem->photoItem().seriesUuid();
  unsigned const focusRow = utils::asserted::fromMap(seriesUuidToRow, focusSeries);
  for (std::size_t i = 0; i < maxNumberOfPhotosInRow(); ++i)
  {
    if (photoInGridAt(focusRow, i) == focusItem)
      return Coords{focusRow, i};
  }

  return utils::asserted::always;
}

std::vector<AllSeries::Coords>
    AllSeries::nextJumpProposals(Coords const& coords, int const directionKey) const
{
  unsigned const row = coords.row;
  unsigned const col = coords.col;

  // need to account for empty rows that should be skipped
  switch(directionKey)
  {
    case Qt::Key_Right:
    {
      auto const nextRow = seriesSet.nonEmpty(row, 1).ord();
      auto const firstRow = seriesSet.nonEmpty(0, 0).ord();
      return {{row, col+1}, {nextRow, 0}, {firstRow, 0}};
    }
    case Qt::Key_Down:
    {
      auto const nextRow = seriesSet.nonEmpty(row, 1).ord();
      auto const firstRow = seriesSet.nonEmpty(0, 0).ord();
      return {{nextRow, col}, {nextRow, Coords::MAX}, {firstRow, col}, {firstRow, Coords::MAX}};
    }
    case Qt::Key_Left:
    {
      auto const prevRow = seriesSet.nonEmpty(row, -1).ord();
      auto const lastRow = seriesSet.lastNonEmpty().ord();
      return {{row, col-1}, {prevRow, Coords::MAX}, {lastRow, Coords::MAX}};
    }
    case Qt::Key_Up:
    {
      auto const prevRow = seriesSet.nonEmpty(row, -1).ord();
      auto const lastRow = seriesSet.lastNonEmpty().ord();
      return {{prevRow, col}, {prevRow, Coords::MAX}, {lastRow, col}, {lastRow, Coords::MAX}};
    }
  }

  return utils::asserted::always;
}

AllSeries::Coords AllSeries::findValidProposal(std::vector<Coords> const& proposals) const
{
  assert(grid->count() > 0); // Should never be called on empty grid

  for (Coords const& c : proposals)
  {
    if (c.row < 0 || c.row >= grid->rowCount())
      continue;
    if (c.col < 0 || (c.col >= static_cast<int>(maxNumberOfPhotosInRow()) && c.col != Coords::MAX))
      continue;
    if (c.col == Coords::MAX)
    {
      for (std::size_t i = 0; i <= maxNumberOfPhotosInRow(); ++i)
        if (!photoInGridAt(c.row, i))
        {
          if (i != 0)
            return {c.row, i-1};
          break;
        }
    }
    else
    {
      if (photoInGridAt(c.row, c.col))
        return {c.row, c.col};
    }
  }

  /*
   * Should never happen if proposals are correctly constructed.
   * On each proposal list should be at least one (0,0) or (0, None). When grid has at least one
   * correctly positioned element at (0,0) it should be found.
   */
  return utils::asserted::always;
}

void AllSeries::changeSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state)
{
  unsigned const seriesRow = utils::asserted::fromMap(seriesUuidToRow, seriesUuid);

  for (int idx = 0; idx < grid->count(); ++idx)
  {
    int r, c, rSpan, cSpan;
    grid->getItemPosition(idx, &r, &c, &rSpan, &cSpan);
    if (r != static_cast<int>(seriesRow))
      continue;

    QLayoutItem *lItem = grid->itemAt(idx);
    QWidget *widgetItem = lItem ? lItem->widget() : nullptr;

    widgets::pitem::PhotoItem* photoWidget = dynamic_cast<widgets::pitem::PhotoItem*>(widgetItem);
    if (!photoWidget)
      continue;

    photoWidget->photoItem().setState(state);
  }
}

void AllSeries::setFocusedWidget(QWidget *widget) const
{
  if (!widget)
    return;
  widget->setFocus();
  scroll->ensureWidgetVisible(widget);
}

}} // namespace phobos::view
