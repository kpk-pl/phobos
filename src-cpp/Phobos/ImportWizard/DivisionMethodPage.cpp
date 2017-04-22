#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QSpinBox>
#include <QRadioButton>
#include "ImportWizard/ImportWizard.h"
#include "ImportWizard/DivisionMethodPage.h"

namespace phobos { namespace importwiz {

DivisionMethodPage::DivisionMethodPage(ImportWizard *parent) :
    QWizardPage(parent), currentSelection(Selection::FixedNum), parentWizard(parent)
{
    setTitle(tr("Division method"));

    numImportedLabel = new QLabel("");
    importMoreButton = new QPushButton(tr("Import more"));
    QObject::connect(importMoreButton, &QPushButton::clicked, this, importMoreFiles);

    QFrame* hline = new QFrame();
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);

    fixedNumChoice = new QRadioButton(tr("Each series has the same number of photos"));
    fixedNumChoice->setChecked(true);
    fixedNumParam = new QSpinBox();
    fixedNumParam->setValue(5);
    fixedNumParam->setSuffix(tr(" photos", "As in '5 photos'"));
    QObject::connect(fixedNumChoice, &QRadioButton::toggled, this, [this]{ updateSelection(Selection::FixedNum); });

    metadataAutoChoice = new QRadioButton(tr("Divide to series automatically based on metadata"));
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
}

void DivisionMethodPage::paintEvent(QPaintEvent *event)
{
    updateLabelText();
    QWizardPage::paintEvent(event);
}

void DivisionMethodPage::importMoreFiles()
{
    parentWizard->loadMoreFiles();
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

void DivisionMethodPage::updateLabelText()
{
    numImportedLabel->setText(tr("Selected %1 photos").arg(parentWizard->selectedFiles().size()));
}

}} // namespace phobos::importwiz
