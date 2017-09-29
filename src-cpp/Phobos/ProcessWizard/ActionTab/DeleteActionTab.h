#ifndef PROCESSWIZARD_ACTIONTAB_DELETEACTIONTAB_H
#define PROCESSWIZARD_ACTIONTAB_DELETEACTIONTAB_H

#include "ProcessWizard/ActionTab/ActionTab.h"
#include "PhotoContainers/ItemState.h"

class QRadioButton;

namespace phobos { namespace processwiz {

class DeleteActionTab : public ActionTab
{
public:
  DeleteActionTab(pcontainer::ItemState const matchState);

private slots:
  void createAction() const;

private:
  QRadioButton *permanentRadio, *trashRadio;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_ACTIONTAB_DELETEACTIONTAB_H
