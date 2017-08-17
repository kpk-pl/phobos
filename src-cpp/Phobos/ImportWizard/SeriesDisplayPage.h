#ifndef IMPORTWIZARD_SERIESDISPLAYPAGE_H
#define IMPORTWIZARD_SERIESDISPLAYPAGE_H

#include <set>
#include <map>
#include <QWizardPage>
#include "ImportWizard/Types.h"

class QTreeWidget;
class QLabel;
class QPushButton;
class QGridLayout;

namespace phobos { namespace widgets {
class TextIconLabel;
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
    QGridLayout* grid;
    QTreeWidget *tree;
    QLabel *loadedStatusLabel;

    using LengthCountMap = std::map<std::size_t, unsigned>;
    void initializeInfoLabels(LengthCountMap const& lengthsCount);

    void initializeLengthOneWarning(std::size_t const count);
    widgets::TextIconLabel *lengthOneWarning;
    QPushButton *selectLengthOneButton;

    void initializeMultipleLengthsInfo(std::set<std::size_t> const& lengths);
    widgets::TextIconLabel *multipleLengthsInfo;

    PhotoSeriesVec _dividedSeries;
    PhotoSeriesVec _chosenSeries;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_SERIESDISPLAYPAGE_H
