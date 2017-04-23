#ifndef IMPORTWIZARD_SERIESDISPLAYPAGE_H
#define IMPORTWIZARD_SERIESDISPLAYPAGE_H

#include <QWizardPage>
#include "ImportWizard/Types.h"

class QTreeWidget;
class QLabel;
class QPushButton;

namespace phobos { namespace widgets {
class IconLabel;
}}

namespace phobos { namespace importwiz {

class SeriesDisplayPage : public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(phobos::importwiz::PhotoSeriesVec chosenSeries MEMBER _chosenSeries READ series NOTIFY seriesChanged)

public:
    SeriesDisplayPage(QWidget* parent = nullptr);

    PhotoSeriesVec series() const { return _chosenSeries; }

signals:
   void seriesChanged(PhotoSeriesVec);

protected:
    void initializePage() override;
    bool validatePage() override;
    void cleanupPage() override;

private slots:
    void selectBackSeriesWithOnePhoto();

private:
    QTreeWidget *tree;

    QLabel *loadedStatusLabel;
    widgets::IconLabel *lengthOneWarning;
    QPushButton *selectLengthOneButton;

    PhotoSeriesVec _dividedSeries;
    PhotoSeriesVec _chosenSeries;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_SERIESDISPLAYPAGE_H
