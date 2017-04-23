#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QSpinBox>
#include <QRadioButton>
#include <easylogging++.h>
#include "ImportWizard/DivisionMethodPage.h"
#include "ImportWizard/ImageOpenDialog.h"
#include "ImportWizard/DivisionOps.h"

namespace phobos { namespace importwiz {

DivisionMethodPage::DivisionMethodPage(QWidget *parent) :
    QWizardPage(parent), currentSelection(Selection::Metadata)
{
    setTitle(tr("Division method"));

    numImportedLabel = new QLabel();
    importMoreButton = new QPushButton(tr("Import more"));
    QObject::connect(importMoreButton, &QPushButton::clicked, this, &DivisionMethodPage::importMoreFiles);

    QFrame* hline = new QFrame();
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);

    fixedNumChoice = new QRadioButton(tr("Each series has the same number of photos"));
    fixedNumParam = new QSpinBox();
    fixedNumParam->setValue(5);
    fixedNumParam->setSuffix(tr(" photos", "As in '5 photos'"));
    fixedNumParam->setDisabled(true);
    fixedNumParam->setMinimum(1);
    QObject::connect(fixedNumChoice, &QRadioButton::toggled, this, [this]{ updateSelection(Selection::FixedNum); });

    metadataAutoChoice = new QRadioButton(tr("Divide to series automatically based on metadata"));
    metadataAutoChoice->setChecked(true);
    QObject::connect(metadataAutoChoice, &QRadioButton::toggled, this, [this]{ updateSelection(Selection::Metadata); });

    QGridLayout *layout = new QGridLayout();
    layout->setColumnStretch(1, 1);
    layout->addWidget(numImportedLabel, 0, 0);
    layout->addWidget(importMoreButton, 0, 2);
    layout->addWidget(hline, 1, 0, 1, -1);
    layout->addWidget(fixedNumChoice, 2, 0, 1, 2);
    layout->addWidget(fixedNumParam, 2, 2);
    layout->addWidget(metadataAutoChoice, 3, 0, 1, 2);
    setLayout(layout);

    registerField("dividedSeries", this, "dividedSeries", SIGNAL(seriesChanged(PhotoSeriesVec)));
}

void DivisionMethodPage::initializePage()
{
    LOG(DEBUG) << "Initializing wizard";
    if (_selectedFiles.empty())
        importMoreFiles();

    if (wizard()->button(QWizard::NextButton))
        wizard()->button(QWizard::NextButton)->setFocus();
    else if (wizard()->button(QWizard::FinishButton))
        wizard()->button(QWizard::FinishButton)->setFocus();
}

void DivisionMethodPage::cleanupPage()
{
    _dividedSeries.clear();
    emit seriesChanged(_dividedSeries);
}

bool DivisionMethodPage::validatePage()
{
    switch(currentSelection)
    {
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

void DivisionMethodPage::importMoreFiles()
{
    LOG(INFO) << "Opening dialog to select additional photos";
    QStringList const newFiles = selectImagesInDialog(this);
    LOG(INFO) << "Selected " << newFiles.size() << " files";

    _selectedFiles.append(newFiles);
    std::sort(_selectedFiles.begin(), _selectedFiles.end());
    _selectedFiles.erase(std::unique(_selectedFiles.begin(), _selectedFiles.end()), _selectedFiles.end());
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
        fixedNumParam->setDisabled(true);
        break;
    }
}

}} // namespace phobos::importwiz
