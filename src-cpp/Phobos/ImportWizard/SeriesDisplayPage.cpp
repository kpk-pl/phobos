#include "ImportWizard/SeriesDisplayPage.h"
#include "ImportWizard/Types.h"
#include "Widgets/IconLabel.h"
#include "Utils/Algorithm.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <easylogging++.h>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <set>

namespace types {
unsigned const SERIES_MASK     = 0x00FF;
unsigned const SERIES_STANDARD = 0x0001;
unsigned const SERIES_ONEPHOTO = 0x0010;
unsigned const PHOTO_MASK      = 0xFF00;
unsigned const PHOTO_STANDARD  = 0x0100;
} // namespace types

namespace phobos { namespace importwiz {

SeriesDisplayPage::SeriesDisplayPage(QWidget *parent) :
  QWizardPage(parent)
{
  setTitle(tr("Confirm series"));

  tree = new QTreeWidget();
  tree->setContextMenuPolicy(Qt::CustomContextMenu);
  tree->header()->close();

  loadedStatusLabel = new QLabel();

  multipleLengthsInfo = new widgets::TextIconLabel(widgets::IconLabel::Icon::Information);
  lengthOneWarning = new widgets::TextIconLabel(widgets::IconLabel::Icon::Warning);
  selectLengthOneButton = new QPushButton(tr("Select back"));

  grid = new QGridLayout();
  grid->setColumnStretch(1, 1);
  grid->addWidget(loadedStatusLabel, 0, 0, 1, -1);
  grid->addWidget(multipleLengthsInfo, 1, 0, 1, 2);
  grid->addWidget(lengthOneWarning, 2, 0, 1, 2);
  grid->addWidget(selectLengthOneButton, 2, 2);
  grid->addWidget(tree, 3, 0, 1, -1);
  setLayout(grid);

  QObject::connect(selectLengthOneButton, &QPushButton::clicked, this, &SeriesDisplayPage::selectBackSeriesWithOnePhoto);
  QObject::connect(tree, &QTreeWidget::customContextMenuRequested, this, &SeriesDisplayPage::treeContextMenu);

  registerField("chosenSeries", this, "chosenSeries", SIGNAL(seriesChanged(PhotoSeriesVec)));
}

void SeriesDisplayPage::initializeInfoLabels()
{
  std::map<std::size_t, unsigned> lengthsCount;
  for (auto const& series : _dividedSeries)
    lengthsCount[series.size()]++;

  auto const oneIt = lengthsCount.find(1);
  if (oneIt != lengthsCount.end())
    initializeLengthOneWarning(oneIt->second);
  else
  {
    lengthOneWarning->hide();
    selectLengthOneButton->hide();
  }

  initializeMultipleLengthsInfo(lengthsCount);
}

void SeriesDisplayPage::initializeLengthOneWarning(std::size_t const count)
{
  LOG(INFO) << count << " series have just one photo";

  lengthOneWarning->show();
  lengthOneWarning->label()->setText(tr("%1 series with only one photo %2 been disabled")
                                        .arg(count).arg(count == 1 ? "has" : "have"));

  selectLengthOneButton->show();
}

void SeriesDisplayPage::initializeMultipleLengthsInfo()
{
  std::map<std::size_t, unsigned> lengthsCount;
  for (auto const& series : _dividedSeries)
    lengthsCount[series.size()]++;

  initializeMultipleLengthsInfo(lengthsCount);
}

void SeriesDisplayPage::initializeMultipleLengthsInfo(std::map<std::size_t, unsigned> const& lengthsCount)
{
  std::set<std::size_t> lengths;
  for (auto const& lc : lengthsCount)
    if (lc.first != 1)
      lengths.insert(lc.first);

  if (lengths.size() < 2)
  {
    multipleLengthsInfo->hide();
    return;
  }

  std::string const sLengthList =
    boost::algorithm::join(lengths | boost::adaptors::transformed(static_cast<std::string(*)(std::size_t)>(std::to_string)), ", ");

  LOG(INFO) << "Detected series with multiple different lengths: " << sLengthList;

  multipleLengthsInfo->show();
  multipleLengthsInfo->label()->setText(tr("Found series with different lengths: %1 photos").arg(sLengthList.c_str()));
}

namespace {
std::unique_ptr<QTreeWidgetItem> makeTreeItem(PhotoSeries const& series)
{
  int const seriesType = (series.size() == 1 ? types::SERIES_ONEPHOTO : types::SERIES_STANDARD);

  std::unique_ptr<QTreeWidgetItem> seriesItem = std::make_unique<QTreeWidgetItem>(seriesType);
  seriesItem->setText(0, QObject::tr("%1 photos").arg(series.size()));
  seriesItem->setFlags(seriesItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate | Qt::ItemIsEnabled);

  for (int i = 0; i < series.size(); ++i)
  {
    QTreeWidgetItem* photoItem = new QTreeWidgetItem(seriesItem.get(), types::PHOTO_STANDARD);
    photoItem->setFlags(photoItem->flags() | Qt::ItemIsUserCheckable);
    photoItem->setText(0, QString("[%1] %2").arg(i).arg(series[i].name));
    photoItem->setCheckState(0, (series.size() > 1) ? Qt::Checked : Qt::Unchecked);
  }

  return seriesItem;
}
} // unnamed namespace

void SeriesDisplayPage::initializePage()
{
  _dividedSeries = field("dividedSeries").value<PhotoSeriesVec>();
  LOG(DEBUG) << "Read " << _dividedSeries.size() << " series from previous dialog";

  std::size_t photoCount = 0;

  for (PhotoSeries const& series : _dividedSeries)
  {
    photoCount += series.size();
    tree->addTopLevelItem(makeTreeItem(series).release());
  }

  loadedStatusLabel->setText(tr("Loaded %1 photos into %2 series").arg(photoCount).arg(_dividedSeries.size()));
  LOG(INFO) << "Displayed " << photoCount  << " photos in " << _dividedSeries.size() << " series";

  initializeInfoLabels();

  if (wizard()->button(QWizard::FinishButton))
    wizard()->button(QWizard::FinishButton)->setFocus();
}

bool SeriesDisplayPage::validatePage()
{
  _chosenSeries.clear();

  assert(_dividedSeries.size() == tree->topLevelItemCount());
  for (int seriesNum = 0; seriesNum < _dividedSeries.size(); ++seriesNum)
  {
    auto const& currentSeries = _dividedSeries[seriesNum];
    auto const& topLevel = tree->topLevelItem(seriesNum);
    assert(currentSeries.size() == topLevel->childCount());

    if (topLevel->checkState(0) == Qt::Checked)
      _chosenSeries.push_back(currentSeries);
    else if (topLevel->checkState(0) == Qt::PartiallyChecked)
    {
      PhotoSeries selectedPhotos;
      selectedPhotos.reserve(currentSeries.size());

      for (int photoNum = 0; photoNum < currentSeries.size(); ++photoNum)
        if (topLevel->child(photoNum)->checkState(0) == Qt::Checked)
          selectedPhotos.push_back(currentSeries[photoNum]);

      assert(!selectedPhotos.empty());
      _chosenSeries.push_back(selectedPhotos);
    }
  }

  LOG(INFO) << "User selected " << _chosenSeries.size() << " series to load";
  emit seriesChanged(_chosenSeries);
  return true;
}

void SeriesDisplayPage::cleanupPage()
{
  QTreeWidgetItem *item;
  while ((item = tree->takeTopLevelItem(0)))
    delete item;

  _dividedSeries.clear();
  _chosenSeries.clear();
  emit seriesChanged(_chosenSeries);
}

void SeriesDisplayPage::selectBackSeriesWithOnePhoto()
{
  LOG(DEBUG) << "Selecting back series with just one photo";
  for (int i = 0; i < tree->topLevelItemCount(); ++i)
    if (tree->topLevelItem(i)->type() == types::SERIES_ONEPHOTO)
      tree->topLevelItem(i)->setCheckState(0, Qt::Checked);

  lengthOneWarning->hide();
  selectLengthOneButton->hide();
}

void SeriesDisplayPage::treeContextMenu(QPoint const& point)
{
  QTreeWidgetItem *item = tree->itemAt(point);
  if (!(item->type() & types::SERIES_MASK))
    return;

  int const itemPosition = tree->indexOfTopLevelItem(item);
  assert(itemPosition >= 0);
  assert(itemPosition < _dividedSeries.size());

  QMenu menu(this);

  if (itemPosition > 0)
  {
    QObject::connect(menu.addAction(tr("Join with previous")), &QAction::triggered, [&]{
      delete tree->takeTopLevelItem(itemPosition);
      delete tree->takeTopLevelItem(itemPosition-1);

      auto const targetIt = std::next(_dividedSeries.begin(), itemPosition-1);
      auto const it = std::next(targetIt, 1);
      utils::moveFromRange(*targetIt, it->begin(), it->end());
      _dividedSeries.erase(it);

      tree->insertTopLevelItem(itemPosition-1, makeTreeItem(*targetIt).release());

      initializeMultipleLengthsInfo();
    });
  }

  if (_dividedSeries[itemPosition].size() > 1)
  {
    QObject::connect(menu.addAction(tr("Split in half")), &QAction::triggered, [&]{
      delete tree->takeTopLevelItem(itemPosition);

      auto const sourceIt = std::next(_dividedSeries.begin(), itemPosition);
      _dividedSeries.insert(itemPosition+1, PhotoSeries{});
      auto const targetIt = std::next(sourceIt);
      auto const sourceItHalf = std::next(sourceIt->begin(), sourceIt->size()/2);
      utils::moveFromRange(*targetIt, sourceItHalf, sourceIt->end());
      sourceIt->erase(sourceItHalf, sourceIt->end());

      tree->insertTopLevelItem(itemPosition, makeTreeItem(*sourceIt).release());
      tree->insertTopLevelItem(itemPosition+1, makeTreeItem(*targetIt).release());

      initializeMultipleLengthsInfo();
    });
  }

  if (menu.actions().empty())
    return;

  menu.exec(tree->mapToGlobal(point));
}

}} // namespace phobos::importwiz
