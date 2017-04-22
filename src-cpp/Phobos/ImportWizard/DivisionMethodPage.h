#ifndef IMPORTWIZARD_DIVISIONMETHODPAGE_H
#define IMPORTWIZARD_DIVISIONMETHODPAGE_H

#include <QWizardPage>

class QLabel;
class QPushButton;
class QSpinBox;
class QRadioButton;

namespace phobos { namespace importwiz {
class ImportWizard;

class DivisionMethodPage : public QWizardPage
{
    Q_OBJECT

public:
    DivisionMethodPage(ImportWizard *parent);

    void paintEvent(QPaintEvent *event) override;

private:
    enum class Selection {
        FixedNum,
        Metadata
    };

    void importMoreFiles();
    void updateLabelText();
    void updateSelection(Selection selection);

    QLabel *numImportedLabel;
    QPushButton *importMoreButton;

    QRadioButton *fixedNumChoice;
    QSpinBox *fixedNumParam;

    QRadioButton *metadataAutoChoice;

    Selection currentSelection;
    ImportWizard *parentWizard;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_DIVISIONMETHODPAGE_H
