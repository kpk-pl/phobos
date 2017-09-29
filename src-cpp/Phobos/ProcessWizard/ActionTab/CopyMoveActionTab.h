#ifndef PROCESSWIZARD_ACTIONTAB_COPYMOVEACTIONTAB_H
#define PROCESSWIZARD_ACTIONTAB_COPYMOVEACTIONTAB_H

#include "ProcessWizard/ActionTab/ActionTab.h"
#include "ProcessWizard/Operation.h"

namespace phobos { namespace widgets {
class FilenameEntry;
}}

class QLabel;
class QCheckBox;

namespace phobos { namespace processwiz {

class ActionsCreatorResources;

class CopyMoveActionTab : public ActionTab
{
public:
  CopyMoveActionTab(pcontainer::ItemState const matchState,
                    OperationType const operation,
                    ActionsCreatorResources &resources);

private slots:
  void createAction() const;
  void selectDirectory();

private:
  OperationType const operation;
  widgets::FilenameEntry *renameWithSyntax;
  QLabel *selectedDirLabel;
  QCheckBox *renameToggle;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_ACTIONTAB_COPYMOVEACTIONTAB_H
