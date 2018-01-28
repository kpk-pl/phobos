#include "AllSeriesView.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/Addon.h"
#include "Widgets/PhotoItem/Capability.h"
#include "Widgets/PhotoItem/Recovery.h"
#include "Widgets/NavigationBar.h"
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

namespace phobos {

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
  seriesSet(seriesSet), imageCache(imageCache), scroll(nullptr), grid(nullptr)
{
  QObject::connect(&imageCache, &icache::Cache::updateMetrics, this, &AllSeriesView::updateMetrics);
  QObject::connect(&seriesSet, &pcontainer::Set::newSeries, this, &AllSeriesView::addNewSeries);
  QObject::connect(&seriesSet, &pcontainer::Set::changedSeries, this, &AllSeriesView::updateExistingSeries);

  prepareUI();
}

void AllSeriesView::prepareUI()
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

  prepareNavigation();

  // TODO: Add some sort of stretch so when each series has one photo and maxHeightLimit takes precedense,
  // photos are aligned to left. The obvious solution to add stretch right to grid layout does not work
  // since stretch has equal precedense to grid and photos are rendered either size 0 or half available space

  QVBoxLayout* newLayout = new QVBoxLayout();
  newLayout->setContentsMargins(5, 5, 0, 0);
  newLayout->addWidget(navigationBar);
  newLayout->addWidget(scroll);

  setLayout(newLayout);
}

void AllSeriesView::prepareNavigation()
{
  navigationBar = new widgets::NavigationBar();
  navigationBar->setContentsMargins(5, 5, 5, 0);

  auto const addButton = [this](std::string const& name, QString const& tooltip, auto && handler)
  {
    auto const btn = navigationBar->addButton(name);
    btn->setToolTip(tooltip);
    QObject::connect(btn, &QPushButton::clicked, this, std::forward<decltype(handler)>(handler));
  };

  addButton("openFolder", tr("Import photos"), &AllSeriesView::importPhotosRequest);
  navigationBar->addSeparator();

  addButton("numSeries", tr("Show side by side photos from one series"),
            [this](){ emit switchView(ViewDescription::make(ViewType::NUM_SINGLE_SERIES)); });
  addButton("oneSeries", tr("Show one series on a single page"),
            [this](){ emit switchView(ViewDescription::make(ViewType::ROW_SINGLE_SERIES)); });
  navigationBar->addButton("singlePhoto")->setToolTip("Open a preview window with only one photo");
  navigationBar->addSeparator();

  addButton("selectBest", tr("Automatically select best photosn each series"), &AllSeriesView::selectBestPhotosRequest);
  navigationBar->addSeparator();

  navigationBar->addButton("movePhotos")->setToolTip("Move selected photos to another folder");
  navigationBar->addButton("deletePhotos")->setToolTip("Remove unselected photos from hard drive");

  navigationBar->addStretch();
  // TODO: counter of selected, unselected photos, series
  // TODO: allow hiding navigationBar to get more space
}

std::size_t AllSeriesView::maxNumberOfPhotosInRow() const
{
  int const cc = grid->columnCount();
  return cc > 0 ? cc-1 : cc;
}

QWidget* AllSeriesView::photoInGridAt(std::size_t const row, std::size_t const col) const
{
  QLayoutItem* wItem = grid->itemAtPosition(row, col+1);
  return wItem ? wItem->widget() : nullptr;
}

void AllSeriesView::addPhotoToGridAt(QWidget *widget, std::size_t const row, std::size_t const col)
{
  grid->addWidget(widget, row, col+1);
}

void AllSeriesView::addNumberingToGrid(int const number)
{
  QLabel *label = new QLabel(QString("%1").arg(number+1));
  label->setContentsMargins(0, 0, 2, 0);

  auto font = label->font();
  font.setBold(true);
  label->setFont(font);

  grid->addWidget(label, number, 0, Qt::AlignRight);
}

void AllSeriesView::focusSeries()
{
  if (numberOfSeries() > 0)
    photoInGridAt(0, 0)->setFocus();
}

void AllSeriesView::focusSeries(QUuid const seriesUuid)
{
  photoInGridAt(utils::asserted::fromMap(seriesUuidToRow, seriesUuid), 0)->setFocus();
}

void AllSeriesView::addNewSeries(pcontainer::SeriesPtr series)
{
  seriesUuidToRow.emplace(series->uuid(), series->ord());

  if (series->empty())
    return;

  addNumberingToGrid(static_cast<int>(series->ord()));

  for (std::size_t col = 0; col < series->size(); ++col)
  {
    pcontainer::ItemPtr const photo = series->item(col);
    addItemToGrid(series->ord(), col, photo);
  }
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

void AllSeriesView::updateExistingSeries(QUuid seriesUuid)
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

void AllSeriesView::addItemToGrid(int const row, int const col, pcontainer::ItemPtr const& itemPtr)
{
  using namespace widgets::pitem;
  static auto const capabilities = CapabilityType::OPEN_SERIES | CapabilityType::REMOVE_PHOTO | CapabilityType::REMOVE_SERIES;

  auto const widgetAddons = Addons(config::get()->get_qualified_array_of<std::string>("allSeriesView.enabledAddons").value_or({}));
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

  QObject::connect(item.get(), &PhotoItem::openInSeries,
    [this](QUuid const& uuid){ switchView(ViewDescription::make(ViewType::ANY_SINGLE_SERIES, uuid)); });

  QObject::connect(item.get(), &PhotoItem::changeSeriesState, this, &AllSeriesView::changeSeriesState);
  QObject::connect(item.get(), &PhotoItem::removeFromSeries, &seriesSet, &pcontainer::Set::removeImage);
  QObject::connect(item.get(), &PhotoItem::removeAllSeries, &seriesSet, &pcontainer::Set::removeSeries);

  addPhotoToGridAt(item.release(), row, col);
}

void AllSeriesView::updateMetrics(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics)
{
  auto& widget = utils::asserted::fromPtr(findItem(itemId));
  widget.setMetrics(metrics);
}

widgets::pitem::PhotoItem* AllSeriesView::findItem(pcontainer::ItemId const& itemId) const
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

void AllSeriesView::keyPressEvent(QKeyEvent* keyEvent)
{
  if (keyEvent->type() == QEvent::KeyPress &&
        utils::valueIn(keyEvent->key(), {Qt::Key_Left, Qt::Key_Right, Qt::Key_Up, Qt::Key_Down}))
  {
    auto const focusCoords = focusGridCoords();
    if (focusCoords)
    {
      Coords const jump = findValidProposal(nextJumpProposals(*focusCoords, keyEvent->key()));
      utils::asserted::fromPtr(photoInGridAt(jump.row, jump.col)).setFocus();
    }
    else
      focusSeries();
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

  unsigned const focusRow = utils::asserted::fromMap(seriesUuidToRow, focusSeries);
  for (std::size_t i = 0; i < maxNumberOfPhotosInRow(); ++i)
  {
    if (photoInGridAt(focusRow, i) == focusItem)
      return Coords{focusRow, i};
  }

  return utils::asserted::always;
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

  return utils::asserted::always;
}

AllSeriesView::Coords AllSeriesView::findValidProposal(std::vector<Coords> const& proposals) const
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

void AllSeriesView::changeSeriesState(QUuid const seriesUuid, pcontainer::ItemState const state)
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

} // namespace phobos
