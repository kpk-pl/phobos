#ifndef IMPORTWIZARD_SERIESDISPLAYPAGE_H
#define IMPORTWIZARD_SERIESDISPLAYPAGE_H

#include "ImportWizard/Types.h"
#include <QWizardPage>
#include <QPoint>
#include <map>

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
  explicit SeriesDisplayPage(QWidget* parent = nullptr);

  PhotoSeriesVec series() const { return _chosenSeries; }

signals:
 void seriesChanged(PhotoSeriesVec);

protected:
  void initializePage() override;
  bool validatePage() override;
  void cleanupPage() override;

private slots:
  void selectBackSeriesWithOnePhoto();
  void splitSuggestedSeries();
  void treeContextMenu(QPoint const& point);

private:
  QGridLayout* grid;
  QTreeWidget *tree;
  QLabel *loadedStatusLabel;

  std::map<std::size_t, unsigned> countSeriesLengths() const;
  void initializeInfoLabels();

  void initializeLengthOneWarning(std::size_t const count);
  widgets::TextIconLabel *lengthOneWarning;
  QPushButton *selectLengthOneButton;

  void initializeMultipleLengthsInfo();
  void initializeMultipleLengthsInfo(std::map<std::size_t, unsigned> const& lengthsCount);
  widgets::TextIconLabel *multipleLengthsInfo;
  widgets::TextIconLabel *suggestedSplitInfo;
  QPushButton *suggestedSplitButton;

  PhotoSeriesVec _dividedSeries;
  PhotoSeriesVec _chosenSeries;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_SERIESDISPLAYPAGE_H
