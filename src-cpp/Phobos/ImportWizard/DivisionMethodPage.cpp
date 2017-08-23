#include "ImportWizard/DivisionMethodPage.h"
#include "ImportWizard/ImageOpenDialog.h"
#include "ImportWizard/DivisionOps.h"
#include "ImportWizard/DateTimeProvider.h"
#include "Widgets/HorizontalLine.h"
#include "Utils/Comparators.h"
#include <easylogging++.h>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QSpinBox>
#include <QRadioButton>
#include <QCheckBox>

namespace phobos { namespace importwiz {

DivisionMethodPage::DivisionMethodPage(QWidget *parent) :
    QWizardPage(parent), fixedNumParamChanged(false), currentSelection(Selection::Metadata)
{
    setTitle(tr("Division method"));

    numImportedLabel = new QLabel();
    importMoreButton = new QPushButton(tr("Import more"));
    QObject::connect(importMoreButton, &QPushButton::clicked, this, &DivisionMethodPage::importMoreFiles);

    fixedNumChoice = new QRadioButton(tr("Each series has the same number of photos"));
    fixedNumParam = new QSpinBox();
    fixedNumParam->setValue(5);
    fixedNumParam->setSuffix(tr(" photos", "As in '5 photos'"));
    fixedNumParam->setDisabled(true);
    fixedNumParam->setMinimum(1);
    QObject::connect(fixedNumParam, &QSpinBox::editingFinished, [this]{ fixedNumParamChanged = true; fixedNumParam->disconnect(); });
    QObject::connect(fixedNumChoice, &QRadioButton::toggled, [this]{ updateSelection(Selection::FixedNum); });

    metadataAutoChoice = new QRadioButton(tr("Divide to series automatically based on metadata"));
    metadataAutoChoice->setChecked(true);
    QObject::connect(metadataAutoChoice, &QRadioButton::toggled, [this]{ updateSelection(Selection::Metadata); });

    noopChoice = new QRadioButton(tr("Don't divide photos - create one series"));
    QObject::connect(noopChoice, &QRadioButton::toggled, [this]{ updateSelection(Selection::DontDivide); });

    notSortedPhotosBox = new QCheckBox(tr("My photos cannot be sorted by file names"));

    QGridLayout *layout = new QGridLayout();
    layout->setColumnStretch(1, 1);
    layout->addWidget(numImportedLabel, 0, 0, 1, 2);
    layout->addWidget(importMoreButton, 0, 2);
    layout->addWidget(new widgets::HorizontalLine(), 1, 0, 1, -1);
    layout->addWidget(fixedNumChoice, 2, 0, 1, 2);
    layout->addWidget(fixedNumParam, 2, 2);
    layout->addWidget(metadataAutoChoice, 3, 0, 1, 2);
    layout->addWidget(noopChoice, 4, 0, 1, 2);
    layout->setRowStretch(5, 1);
    layout->addWidget(notSortedPhotosBox, 5, 0, 1, 3, Qt::AlignBottom | Qt::AlignLeft);
    setLayout(layout);

    registerField("dividedSeries", this, "dividedSeries", SIGNAL(seriesChanged(PhotoSeriesVec)));

    // TODO: Provide input box to enter minimal number of photos in series. On next page automatically deselect those in the existing way.
    // Update warning text
}

void DivisionMethodPage::initializePage()
{
    LOG(DEBUG) << "Initializing wizard";
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

  if (notSortedPhotosBox->isChecked())
  {
    LOG(INFO) << "Sorting photos based on time before division";
    std::sort(sortedPhotos.begin(), sortedPhotos.end(), utils::less().on(&Photo::lastModTime));
  }

  switch(currentSelection)
  {
  case Selection::DontDivide:
    LOG(INFO) << "Dividing photos as noop";
    _dividedSeries = divideToSeriesNoop(std::move(sortedPhotos));
    break;
  case Selection::FixedNum:
    LOG(INFO) << "Dividing photos to series with equal size of " << fixedNumParam->value();
    _dividedSeries = divideToSeriesWithEqualSize(std::move(sortedPhotos), fixedNumParam->value());
    break;
  case Selection::Metadata:
    LOG(INFO) << "Dividing photos based on metadata";
    _dividedSeries = divideToSeriesOnMetadata(std::move(sortedPhotos));
    break;
  }

  LOG(INFO) << "Divided into " << _dividedSeries.size() << " series";
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
} // unnamed namespace

void DivisionMethodPage::importMoreFiles()
{
  LOG(INFO) << "Opening dialog to select additional photos";

  QStringList newFiles = selectImagesInDialog(this);
  LOG(INFO) << "Selected " << newFiles.size() << " new files";

  auto newPhotos = provideDateTime(newFiles);

  for (auto & newPhoto : newPhotos)
  {
    auto const ub = std::lower_bound(_selectedFiles.begin(), _selectedFiles.end(), newPhoto, utils::less().on(&Photo::fileName));
    if (ub == _selectedFiles.end())
      _selectedFiles.insert(_selectedFiles.end(), std::move(newPhoto));
    else if (!utils::equal().on(&Photo::fileName)(*ub, newPhoto))
      _selectedFiles.insert(std::next(ub), std::move(newPhoto));
  }

  if (!fixedNumParamChanged)
    fixedNumParam->setValue(guessBestDivisionValue(_selectedFiles.size()));

  LOG(INFO) << "Processing " << _selectedFiles.size() << " photos in total in current wizard";
  numImportedLabel->setText(tr("Selected %1 photos").arg(_selectedFiles.size()));
  update();
}

void DivisionMethodPage::updateSelection(Selection selection)
{
  currentSelection = selection;

  switch(currentSelection)
  {
  case Selection::FixedNum:
    fixedNumParam->setDisabled(false);
    break;
  case Selection::Metadata:
  case Selection::DontDivide:
    fixedNumParam->setDisabled(true);
    break;
  }
}

}} // namespace phobos::importwiz
