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
  setTitle(tr("Confirm division process"));

  tree = new QTreeWidget();
  tree->setContextMenuPolicy(Qt::CustomContextMenu);
  tree->header()->close();

  loadedStatusLabel = new QLabel();

  multipleLengthsInfo = new widgets::TextIconLabel(widgets::IconLabel::Icon::Information);

  suggestedSplitInfo = new widgets::TextIconLabel(widgets::IconLabel::Icon::Information, tr("Some groups might not have been correctly splitted"));
  suggestedSplitButton = new QPushButton(tr("Split now"));

  lengthOneWarning = new widgets::TextIconLabel(widgets::IconLabel::Icon::Warning);
  selectLengthOneButton = new QPushButton(tr("Select back"));

  grid = new QGridLayout();
  grid->setColumnStretch(1, 1);
  grid->addWidget(loadedStatusLabel, 0, 0, 1, -1);
  grid->addWidget(multipleLengthsInfo, 1, 0, 1, 2);
  grid->addWidget(suggestedSplitInfo, 2, 0, 1, 2);
  grid->addWidget(suggestedSplitButton, 2, 2);
  grid->addWidget(lengthOneWarning, 3, 0, 1, 2);
  grid->addWidget(selectLengthOneButton, 3, 2);
  grid->addWidget(tree, 4, 0, 1, -1);
  setLayout(grid);

  QObject::connect(suggestedSplitButton, &QPushButton::clicked, this, &SeriesDisplayPage::splitSuggestedSeries);
  QObject::connect(selectLengthOneButton, &QPushButton::clicked, this, &SeriesDisplayPage::selectBackSeriesWithOnePhoto);
  QObject::connect(tree, &QTreeWidget::customContextMenuRequested, this, &SeriesDisplayPage::treeContextMenu);

  registerField("chosenSeries", this, "chosenSeries", SIGNAL(seriesChanged(PhotoSeriesVec)));
}

std::map<std::size_t, unsigned> SeriesDisplayPage::countSeriesLengths() const
{
  std::map<std::size_t, unsigned> lengthsCount;
  for (auto const& series : _dividedSeries)
    if (series.isASeries)
      lengthsCount[series.size()]++;

  return lengthsCount;
}

void SeriesDisplayPage::initializeInfoLabels()
{
  auto const lenghtsCount = countSeriesLengths();

  auto const oneIt = lenghtsCount.find(1);
  if (oneIt != lenghtsCount.end() && lenghtsCount.size() > 1)
  {
    initializeLengthOneWarning(oneIt->second);
  }
  else
  {
    lengthOneWarning->hide();
    selectLengthOneButton->hide();
  }

  initializeMultipleLengthsInfo(lenghtsCount);
}

void SeriesDisplayPage::initializeLengthOneWarning(std::size_t const count)
{
  LOG(TRACE) << count << " groups have just one photo";

  lengthOneWarning->show();
  lengthOneWarning->label()->setText(tr("%1 groups with only one photo %2 been disabled")
                                        .arg(count).arg(count == 1 ? "has" : "have"));

  selectLengthOneButton->show();
}

void SeriesDisplayPage::initializeMultipleLengthsInfo()
{
  initializeMultipleLengthsInfo(countSeriesLengths());
}

namespace {
std::set<std::size_t> keysFromMap(std::map<std::size_t, unsigned> const& map)
{
  std::set<std::size_t> result;

  for (auto const& lc : map)
    result.insert(lc.first);

  return result;
}

std::map<std::size_t, std::size_t> dividingPairs(std::set<std::size_t> const& nums)
{
  std::map<std::size_t, std::size_t> result;
  if (nums.size() < 2)
    return result;

  for (auto rightIt = std::next(nums.begin()); rightIt != nums.end(); ++rightIt)
    for (auto leftIt = nums.begin(); leftIt != rightIt; ++leftIt)
      if (*rightIt % *leftIt == 0)
      {
        result.emplace(*rightIt, *leftIt);
        break;
      }

  return result;
}
} // unnamed namespace

void SeriesDisplayPage::initializeMultipleLengthsInfo(std::map<std::size_t, unsigned> const& lengthsCount)
{
  std::set<std::size_t> lengths = keysFromMap(lengthsCount);
  lengths.erase(1);

  if (lengths.size() < 2 || std::none_of(_dividedSeries.begin(), _dividedSeries.end(), [](PhotoSeries const& ps){ return ps.isASeries; }))
  {
    multipleLengthsInfo->hide();
    suggestedSplitInfo->hide();
    suggestedSplitButton->hide();
    return;
  }

  std::string const sLengthList =
    boost::algorithm::join(lengths | boost::adaptors::transformed(static_cast<std::string(*)(std::size_t)>(std::to_string)), ", ");

  LOG(TRACE) << "Detected groups with multiple different lengths: " << sLengthList;

  multipleLengthsInfo->label()->setText(tr("Found groups with different lengths: %1 photos").arg(sLengthList.c_str()));
  multipleLengthsInfo->show();

  auto const possibleDivs = dividingPairs(lengths);
  if (!possibleDivs.empty())
  {
    suggestedSplitButton->show();
    suggestedSplitInfo->show();
  }
  else
  {
    suggestedSplitButton->hide();
    suggestedSplitInfo->hide();
  }
}

namespace {
std::unique_ptr<QTreeWidgetItem> makeTreeItem(PhotoSeries const& series)
{
  int const seriesType = ((series.size() == 1 && series.isASeries) ? types::SERIES_ONEPHOTO : types::SERIES_STANDARD);

  std::unique_ptr<QTreeWidgetItem> seriesItem = std::make_unique<QTreeWidgetItem>(seriesType);
  seriesItem->setText(0, QObject::tr("%1 photos").arg(series.size()));
  seriesItem->setFlags(seriesItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate | Qt::ItemIsEnabled);

  for (int i = 0; i < series.size(); ++i)
  {
    QTreeWidgetItem* photoItem = new QTreeWidgetItem(seriesItem.get(), types::PHOTO_STANDARD);
    photoItem->setFlags(photoItem->flags() | Qt::ItemIsUserCheckable);
    photoItem->setText(0, QString("[%1] %2").arg(i).arg(series[i].name));
    photoItem->setCheckState(0, (seriesType == types::SERIES_STANDARD) ? Qt::Checked : Qt::Unchecked);
  }

  return seriesItem;
}
} // unnamed namespace

void SeriesDisplayPage::initializePage()
{
  _dividedSeries = field("dividedSeries").value<PhotoSeriesVec>();
  LOG(DEBUG) << "Read " << _dividedSeries.size() << " groups from previous dialog";

  initializeNewSeries();

  if (wizard()->button(QWizard::FinishButton))
    wizard()->button(QWizard::FinishButton)->setFocus();
}

void SeriesDisplayPage::initializeNewSeries()
{
  std::size_t photoCount = 0;

  for (PhotoSeries const& series : _dividedSeries)
  {
    photoCount += series.size();
    tree->addTopLevelItem(makeTreeItem(series).release());
  }

  loadedStatusLabel->setText(tr("Loaded %1 photos into %2 groups").arg(photoCount).arg(_dividedSeries.size()));
  LOG(DEBUG) << "Displayed " << photoCount  << " photos in " << _dividedSeries.size() << " groups";

  initializeInfoLabels();
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

  LOG(TRACE) << "User selected " << _chosenSeries.size() << " groups to load";
  emit seriesChanged(_chosenSeries);
  return true;
}

void SeriesDisplayPage::cleanupPage()
{
  silentCleanup();
  emit seriesChanged(_chosenSeries);
}

void SeriesDisplayPage::silentCleanup()
{
  QTreeWidgetItem *item;
  while ((item = tree->takeTopLevelItem(0)))
    delete item;

  _dividedSeries.clear();
  _chosenSeries.clear();
}

void SeriesDisplayPage::selectBackSeriesWithOnePhoto()
{
  LOG(TRACE) << "Selecting back groups with just one photo";
  for (int i = 0; i < tree->topLevelItemCount(); ++i)
    if (tree->topLevelItem(i)->type() == types::SERIES_ONEPHOTO)
      tree->topLevelItem(i)->setCheckState(0, Qt::Checked);

  lengthOneWarning->hide();
  selectLengthOneButton->hide();
}

void SeriesDisplayPage::splitSuggestedSeries()
{
  LOG(TRACE) << "Splitting groups by suggestions";

  std::set<std::size_t> lengths = keysFromMap(countSeriesLengths());
  lengths.erase(1);
  auto const possibleDivs = dividingPairs(lengths);

  PhotoSeriesVec newSeries;

  for (auto oldSeries : _dividedSeries)
  {
    auto const rule = possibleDivs.find(oldSeries.length());
    if (rule == possibleDivs.end())
    {
      newSeries.push_back(std::move(oldSeries));
      continue;
    }

    for (auto it = oldSeries.begin(); it != oldSeries.end(); )
    {
      auto const endIt = std::next(it, rule->second);
      newSeries.push_back(PhotoSeries{});
      utils::moveFromRange(newSeries.back(), it, endIt);
      it = endIt;
    }
  }

  silentCleanup();

  _dividedSeries = newSeries;
  initializeNewSeries();
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
      LOG(TRACE) << "Manual joining groups at position " << itemPosition << " with previous";

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
      LOG(TRACE) << "Manual splitting groups in half at position " << itemPosition;

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
