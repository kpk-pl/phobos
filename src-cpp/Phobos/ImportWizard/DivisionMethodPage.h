#ifndef IMPORTWIZARD_DIVISIONMETHODPAGE_H
#define IMPORTWIZARD_DIVISIONMETHODPAGE_H

#include <QWizardPage>
#include <QStringList>
#include "ImportWizard/Types.h"

class QLabel;
class QPushButton;
class QSpinBox;
class QRadioButton;

namespace phobos { namespace importwiz {

class DivisionMethodPage : public QWizardPage
{
    Q_OBJECT

public:
    DivisionMethodPage(QWidget *parent);

    std::vector<PhotoSeries> const& series() const { return _dividedSeries; }

protected:
    bool validatePage() override;
    void initializePage() override;
    void cleanupPage() override;

private:
    enum class Selection {
        FixedNum,
        Metadata
    };

    void importMoreFiles();
    void updateSelection(Selection selection);

    QLabel *numImportedLabel;
    QPushButton *importMoreButton;

    QRadioButton *fixedNumChoice;
    QSpinBox *fixedNumParam;

    QRadioButton *metadataAutoChoice;

    Selection currentSelection;
    QStringList _selectedFiles;
    std::vector<PhotoSeries> _dividedSeries;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_DIVISIONMETHODPAGE_H
