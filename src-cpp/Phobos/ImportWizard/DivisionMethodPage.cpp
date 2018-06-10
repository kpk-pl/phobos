#include "ImportWizard/DivisionMethodPage.h"
#include "ImportWizard/ImageOpenDialog.h"
#include "ImportWizard/DivisionOps.h"
#include "ImportWizard/FileInfoProvider.h"
#include "Widgets/HVLine.h"
#include "Utils/Comparators.h"
#include <easylogging++.h>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QSpinBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>

namespace phobos { namespace importwiz {

DivisionMethodPage::DivisionMethodPage(QWidget *parent) :
    QWizardPage(parent), fixedNumParamChanged(false), currentSelection(Selection::Metadata)
{
  setTitle(tr("Division method"));

  numImportedLabel = new QLabel();

  QPushButton *importMoreButton = new QPushButton(tr("Import more"));
  QObject::connect(importMoreButton, &QPushButton::clicked, this, &DivisionMethodPage::importMoreFiles);

  QRadioButton *fixedNumChoice = new QRadioButton(tr("Each series has the same number of photos"));
  fixedNumParam = new QSpinBox();
  fixedNumParam->setValue(5);
  fixedNumParam->setSuffix(tr(" photos", "As in '5 photos'"));
  fixedNumParam->setDisabled(true);
  fixedNumParam->setMinimum(1);
  fixedNumParam->setToolTip(tr("Number of photos in each group after they are loaded into application"));
  // disconnect after first edit, so user input is unchanged automatically
  QObject::connect(fixedNumParam, &QSpinBox::editingFinished, [this]{ fixedNumParamChanged = true; fixedNumParam->disconnect(); });
  QObject::connect(fixedNumChoice, &QRadioButton::toggled, [this]{ updateSelection(Selection::FixedNum); });

  QRadioButton *notASeriesChoice = new QRadioButton(tr("There are no series - photos are not related to each other"));
  QObject::connect(notASeriesChoice, &QRadioButton::toggled, [this]{ updateSelection(Selection::NotASeries); });

  QRadioButton *metadataAutoChoice = new QRadioButton(tr("Divide to series automatically based on metadata"));
  metadataAutoChoice->setChecked(true);
  QObject::connect(metadataAutoChoice, &QRadioButton::toggled, [this]{ updateSelection(Selection::Metadata); });

  QRadioButton *noopChoice = new QRadioButton(tr("Don't divide photos - create one series"));
  QObject::connect(noopChoice, &QRadioButton::toggled, [this]{ updateSelection(Selection::DontDivide); });

  sortingMethod = new QComboBox;
  sortingMethod->addItems({"Sort by filename (preferred)", "Sort by date"});
  sortingMethod->setToolTip(tr("Sort by date if photos have irregular naming pattern, are renamed or come from several different cameras"));

  QGridLayout *layout = new QGridLayout();
  layout->setColumnStretch(1, 1);
  layout->addWidget(numImportedLabel, 0, 0, 1, 2);
  layout->addWidget(importMoreButton, 0, 2);
  layout->addWidget(new widgets::HVLine(Qt::Horizontal), 1, 0, 1, -1);
  layout->addWidget(fixedNumChoice, 2, 0, 1, 2);
  layout->addWidget(fixedNumParam, 2, 2);
  layout->addWidget(notASeriesChoice, 3, 0, 1, 2);
  layout->addWidget(metadataAutoChoice, 4, 0, 1, 2);
  layout->addWidget(noopChoice, 5, 0, 1, 2);
  layout->setRowStretch(6, 1);
  layout->addWidget(sortingMethod, 6, 0, 1, 3, Qt::AlignBottom | Qt::AlignLeft);
  setLayout(layout);

  registerField("dividedSeries", this, "dividedSeries", SIGNAL(seriesChanged(PhotoSeriesVec)));
}

void DivisionMethodPage::initializePage()
{
  LOG(DEBUG) << "Initializing division method page";
  if (_selectedFiles.empty())
      importMoreFiles();

  if (wizard()->button(QWizard::NextButton))
    wizard()->button(QWizard::NextButton)->setFocus();
}

void DivisionMethodPage::cleanupPage()
{
  _dividedSeries.clear();
  emit seriesChanged(_dividedSeries);
}

bool DivisionMethodPage::validatePage()
{
  std::vector<Photo> sortedPhotos = _selectedFiles;

  if (sortingMethod->currentIndex() > 0)
  {
    LOG(TRACE) << "Sorting photos based on time before division";
    std::sort(sortedPhotos.begin(), sortedPhotos.end(), Photo::TimeNameComp{});
  }

  switch(currentSelection)
  {
  case Selection::DontDivide:
    LOG(TRACE) << "Dividing photos as noop";
    _dividedSeries = divideToSeriesNoop(std::move(sortedPhotos));
    break;
  case Selection::FixedNum:
  case Selection::NotASeries:
    LOG(TRACE) << "Dividing photos to chunks with equal size of " << fixedNumParam->value();
    _dividedSeries = divideToSeriesWithEqualSize(std::move(sortedPhotos), fixedNumParam->value());
    if (currentSelection == Selection::NotASeries)
    {
      LOG(TRACE) << "Loaded chunks does not form any series";
      std::for_each(_dividedSeries.begin(), _dividedSeries.end(), [](PhotoSeries &series){ series.isASeries = false; });
    }
    break;
  case Selection::Metadata:
    LOG(TRACE) << "Dividing photos based on metadata";
    _dividedSeries = divideToSeriesOnMetadata(std::move(sortedPhotos));
    break;
  }

  LOG(TRACE) << "Divided into " << _dividedSeries.size() << " groups";
  emit seriesChanged(_dividedSeries);
  return true;
}

namespace {
std::size_t guessBestDivisionValue(std::size_t const size)
{
  for (auto const guess : {13, 11, 7, 5, 3, 12, 10, 8, 4})
    if (size % guess == 0)
      return guess;
  return 5;
}

struct PhotoNameView
{
  QString const& name;
  PhotoNameView(QString const& pName) : name(pName)
  {}
  PhotoNameView(Photo const& photo) : name(photo.name)
  {}
  bool operator<(PhotoNameView const& other)
  {
    return name < other.name;
  }
};

QStringList importMoreUniqueFiles(std::vector<Photo> const& currentSelection, QWidget *parent)
{
  LOG(TRACE) << "Opening dialog to select additional photos to import";

  QStringList newFiles = selectImagesInDialog(parent);
  LOG(TRACE) << "Selected " << newFiles.size() << " new files";

  if (newFiles.empty())
    return {};

  QStringList newUniqueFiles;
  newUniqueFiles.reserve(newFiles.size());

  std::sort(newFiles.begin(), newFiles.end());
  std::set_difference(newFiles.begin(), newFiles.end(),
                      currentSelection.begin(), currentSelection.end(),
                      std::back_inserter(newUniqueFiles),
                      [](PhotoNameView lhs, PhotoNameView const& rhs){ return lhs < rhs; });

  LOG(TRACE) << "Filtered selection to " << newUniqueFiles.size() << " unique new files";
  return newUniqueFiles;
}
} // unnamed namespace

void DivisionMethodPage::importMoreFiles()
{
  QStringList newFiles = importMoreUniqueFiles(_selectedFiles, this);
  if (newFiles.empty())
    return;

  auto newPhotos = provideFileInfo(newFiles, this);

  std::size_t const previousSize = _selectedFiles.size();

  _selectedFiles.reserve(previousSize + newPhotos.size());
  std::move(newPhotos.begin(), newPhotos.end(), std::back_inserter(_selectedFiles));
  std::inplace_merge(_selectedFiles.begin(), std::next(_selectedFiles.begin(), previousSize), _selectedFiles.end(),
                     utils::less().on(&Photo::name));

  updateNumBoxWithGuess();

  LOG(TRACE) << "Processing " << _selectedFiles.size() << " photos in total in current wizard";
  numImportedLabel->setText(tr("Selected %1 photos").arg(_selectedFiles.size()));
  update();
}

void DivisionMethodPage::updateNumBoxWithGuess()
{
  if (fixedNumParamChanged)
    return;

  if (currentSelection == Selection::NotASeries)
    fixedNumParam->setValue(5);
  else
    fixedNumParam->setValue(guessBestDivisionValue(_selectedFiles.size()));
}

void DivisionMethodPage::updateSelection(Selection selection)
{
  currentSelection = selection;

  switch(currentSelection)
  {
  case Selection::FixedNum:
  case Selection::NotASeries:
    fixedNumParam->setDisabled(false);
    updateNumBoxWithGuess();
    break;
  case Selection::Metadata:
  case Selection::DontDivide:
    fixedNumParam->setDisabled(true);
    break;
  }
}

}} // namespace phobos::importwiz
