#ifndef PROCESSWIZARD_ACTIONS_CREATOR_PAGE_H
#define PROCESSWIZARD_ACTIONS_CREATOR_PAGE_H

#include "ProcessWizard/Action.h"
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
  Q_PROPERTY(phobos::processwiz::ConstActionPtrVec chosenActions READ chosenActions NOTIFY actionsChanged STORED false)

public:
  ActionsCreatorPage(OperationType const& defaultOperation);

  ConstActionPtrVec chosenActions() const;

signals:
  void actionsChanged(ConstActionPtrVec);

protected:
  void initializePage() override;
  void cleanupPage() override;

private slots:
  void selectedActionsChanged();

private:
  QTabWidget *actionTabsWidget;
  std::map<pcontainer::ItemState, TypeActionTab*> actionTabs;

  QLabel *statusLabel;
  QPushButton *resetButton;

  void addStatusRow(QVBoxLayout *layout);
  void addTypeTabs(QVBoxLayout *layout, OperationType const& defaultOperation);
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_ACTIONS_CREATOR_PAGE_H
