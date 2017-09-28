#ifndef PROCESSWIZARD_TYPEACTIONTAB_H
#define PROCESSWIZARD_TYPEACTIONTAB_H

#include "PhotoContainers/ItemState.h"
#include "ProcessWizard/Action.h"
#include "ProcessWizard/Operation.h"
#include <QWidget>
#include <map>

class QListWidget;
class QTabWidget;

namespace phobos { namespace processwiz {

class ActionTab;
class ActionsCreatorResources;

class TypeActionTab : public QWidget
{
  Q_OBJECT

public:
  TypeActionTab(pcontainer::ItemState const matchState, ActionsCreatorResources &resources);

  ConstActionPtrVec activeActions() const;
  void setCurrentTab(OperationType const& operation) const;

signals:
  void actionsChanged();

public slots:
  void clearActions();

private slots:
  void acceptNewAction(ConstActionPtr action);
  void selectivelyDisableActions();

private:
  using ActionWithOrder = std::pair<ConstActionPtr, std::size_t>;
  struct ActionWithOrderComp;

  QTabWidget *operationTabsWidget;
  std::map<OperationType, ActionTab*> operationTabs;

  std::vector<ActionWithOrder> actions;
  QListWidget *listWidget;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_TYPEACTIONTAB_H
