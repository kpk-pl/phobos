#ifndef IMPORTWIZARD_DIVISIONMETHODPAGE_H
#define IMPORTWIZARD_DIVISIONMETHODPAGE_H

#include "ImportWizard/Types.h"
#include <QWizardPage>
#include <vector>

class QLabel;
class QPushButton;
class QSpinBox;
class QRadioButton;
class QCheckBox;

namespace phobos { namespace importwiz {

class DivisionMethodPage : public QWizardPage
{
    Q_OBJECT

    // although docs does not mention the namespace needs to be fully specified, it really must be
    Q_PROPERTY(phobos::importwiz::PhotoSeriesVec dividedSeries MEMBER _dividedSeries READ series NOTIFY seriesChanged)

public:
    explicit DivisionMethodPage(QWidget *parent = nullptr);

    PhotoSeriesVec series() const { return _dividedSeries; }

signals:
    void seriesChanged(PhotoSeriesVec);

protected:
    bool validatePage() override;
    void initializePage() override;
    void cleanupPage() override;

private:
    enum class Selection {
        DontDivide,
        FixedNum,
        Metadata
    };

    void importMoreFiles();
    void updateSelection(Selection selection);

    QLabel *numImportedLabel;
    QPushButton *importMoreButton;

    QRadioButton *fixedNumChoice;
    QSpinBox *fixedNumParam;
    bool fixedNumParamChanged;

    QRadioButton *metadataAutoChoice;
    QRadioButton *noopChoice;

    QCheckBox *notSortedPhotosBox;

    Selection currentSelection;
    std::vector<Photo> _selectedFiles;
    PhotoSeriesVec _dividedSeries;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_DIVISIONMETHODPAGE_H
