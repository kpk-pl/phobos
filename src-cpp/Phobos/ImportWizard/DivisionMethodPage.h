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

// http://stackoverflow.com/questions/41040906/qwizardpage-registerfield-for-class-property-not-working

class DivisionMethodPage : public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(PhotoSeriesVec dividedSeries MEMBER _dividedSeries READ series NOTIFY seriesChanged)

public:
    DivisionMethodPage(QWidget *parent = nullptr);

    PhotoSeriesVec series() const { return _dividedSeries; }

signals:
    void seriesChanged(PhotoSeriesVec);

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
    PhotoSeriesVec _dividedSeries;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_DIVISIONMETHODPAGE_H
