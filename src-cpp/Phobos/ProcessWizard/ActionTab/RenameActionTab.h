#ifndef PROCESSWIZARD_ACTIONTAB_RENAMEACTIONTAB_H
#define PROCESSWIZARD_ACTIONTAB_RENAMEACTIONTAB_H

#include "ProcessWizard/ActionTab/ActionTab.h"

namespace phobos { namespace widgets {
class FilenameEntry;
}}

namespace phobos { namespace processwiz {

class ActionsCreatorResources;

class RenameActionTab : public ActionTab
{
public:
  RenameActionTab(pcontainer::ItemState const matchState,
                  ActionsCreatorResources &resources);

private slots:
  void createAction() const;

private:
  widgets::FilenameEntry *renameWithSyntax;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_ACTIONTAB_RENAMEACTIONTAB_H
