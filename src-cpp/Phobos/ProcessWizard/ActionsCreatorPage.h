#ifndef PROCESSWIZARD_ACTIONS_CREATOR_PAGE_H
#define PROCESSWIZARD_ACTIONS_CREATOR_PAGE_H

#include "PhotoContainers/Fwd.h"
#include "ProcessWizard/Action.h"
#include "ProcessWizard/SeriesCounts.h"
#include <QWizardPage>

class QVBoxLayout;
class QPushButton;
class QLabel;

namespace phobos { namespace processwiz {

class TypeActionTab;

class ActionsCreatorPage : public QWizardPage
{
  Q_OBJECT

public:
  ActionsCreatorPage(pcontainer::Set const& seriesSet, SeriesCounts const& counts);

protected:
  void initializePage() override;

private slots:
  void updateStatusLabel();

private:
  SeriesCounts const& seriesCounts;
  std::vector<TypeActionTab*> actionTabs;

  QLabel *statusLabel;
  QPushButton *resetButton;

  void addStatusRow(QVBoxLayout *layout);
  void addTypeTabs(QVBoxLayout *layout);
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_ACTIONS_CREATOR_PAGE_H
