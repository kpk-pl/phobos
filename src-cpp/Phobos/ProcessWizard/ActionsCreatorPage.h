#ifndef PROCESSWIZARD_ACTIONS_CREATOR_PAGE_H
#define PROCESSWIZARD_ACTIONS_CREATOR_PAGE_H

#include "ProcessWizard/Action.h"
#include "ProcessWizard/SeriesCounts.h"
#include "ProcessWizard/Operation.h"
#include <QWizardPage>

class QVBoxLayout;
class QPushButton;
class QTabWidget;
class QLabel;

namespace phobos { namespace processwiz {

class TypeActionTab;

class ActionsCreatorPage : public QWizardPage
{
  Q_OBJECT

public:
  ActionsCreatorPage(SeriesCounts const& counts, OperationType const& defaultOperation);

protected:
  void initializePage() override;

private slots:
  void updateStatusLabel();

private:
  SeriesCounts const& seriesCounts;

  QTabWidget *actionTabsWidget;
  std::map<pcontainer::ItemState, TypeActionTab*> actionTabs;

  QLabel *statusLabel;
  QPushButton *resetButton;

  void addStatusRow(QVBoxLayout *layout);
  void addTypeTabs(QVBoxLayout *layout);
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_ACTIONS_CREATOR_PAGE_H
