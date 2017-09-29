#ifndef PROCESSWIZARD_ACTIONTAB_ACTIONTAB_H
#define PROCESSWIZARD_ACTIONTAB_ACTIONTAB_H

#include "ProcessWizard/Operation.h"
#include "ProcessWizard/ActionFwd.h"
#include "PhotoContainers/ItemState.h"
#include <QWidget>
#include <memory>

namespace phobos { namespace processwiz {

class ActionsCreatorResources;

class ActionTab : public QWidget
{
  Q_OBJECT

public:
  static std::unique_ptr<ActionTab> create(OperationType const operation,
                                           pcontainer::ItemState const matchState,
                                           ActionsCreatorResources &resources);

  ActionTab(pcontainer::ItemState const matchState);

signals:
  void newAction(ConstActionPtr) const;

protected:
  pcontainer::ItemState const matchState;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_ACTIONTAB_ACTIONTAB_H
