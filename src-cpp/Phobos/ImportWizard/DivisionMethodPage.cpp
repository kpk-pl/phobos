#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QSpinBox>
#include <QRadioButton>
#include <easylogging++.h>
#include "ImportWizard/DivisionMethodPage.h"
#include "ImportWizard/ImageOpenDialog.h"
#include "ImportWizard/DivisionOps.h"
#include "ImportWizard/DateTimeProvider.h"
#include "Widgets/HorizontalLine.h"
#include "Utils/Comparators.h"

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

    QGridLayout *layout = new QGridLayout();
    layout->setColumnStretch(1, 1);
    layout->addWidget(numImportedLabel, 0, 0);
    layout->addWidget(importMoreButton, 0, 2);
    layout->addWidget(new widgets::HorizontalLine(), 1, 0, 1, -1);
    layout->addWidget(fixedNumChoice, 2, 0, 1, 2);
    layout->addWidget(fixedNumParam, 2, 2);
    layout->addWidget(metadataAutoChoice, 3, 0, 1, 2);
    layout->addWidget(noopChoice, 4, 0, 1, 2);
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
  // TODO: Sort order must be configurable. Definetely filename is not enough. Need to use date or EXIF date.
  switch(currentSelection)
  {
  case Selection::DontDivide:
    LOG(INFO) << "Dividing photos as noop";
    _dividedSeries = divideToSeriesNoop(_selectedFiles);
    break;
  case Selection::FixedNum:
    LOG(INFO) << "Dividing photos to series with equal size of " << fixedNumParam->value();
    _dividedSeries = divideToSeriesWithEqualSize(_selectedFiles, fixedNumParam->value());
    break;
  case Selection::Metadata:
    LOG(INFO) << "Dividing photos based on metadata";
    _dividedSeries = divideToSeriesOnMetadata(_selectedFiles);
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
  auto const fileNameProj = [](Photo const& p){ return p.fileName; };
  auto const lessComp = utils::less().on(fileNameProj);
  auto const eqComp = utils::equal().on(fileNameProj);

  for (auto & newPhoto : newPhotos)
  {
    auto const ub = std::lower_bound(_selectedFiles.begin(), _selectedFiles.end(), newPhoto, lessComp);
    if (ub == _selectedFiles.end())
      _selectedFiles.insert(_selectedFiles.end(), std::move(newPhoto));
    else if (!eqComp(*ub, newPhoto))
      _selectedFiles.insert(std::next(ub), std::move(newPhoto));
  }

  for (auto const& x : _selectedFiles)
    LOG(DEBUG) << x.fileName;

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
