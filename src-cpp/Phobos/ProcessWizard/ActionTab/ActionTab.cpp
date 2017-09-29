#include "ProcessWizard/ActionTab/ActionTab.h"
#include "ProcessWizard/ActionTab/DeleteActionTab.h"
#include "ProcessWizard/ActionTab/RenameActionTab.h"
#include "ProcessWizard/ActionTab/CopyMoveActionTab.h"
#include <cassert>

namespace phobos { namespace processwiz {

std::unique_ptr<ActionTab> ActionTab::create(OperationType const operation,
                                             pcontainer::ItemState const matchState,
                                             ActionsCreatorResources &resources)
{
  switch(operation)
  {
  case OperationType::Delete:
    return std::make_unique<DeleteActionTab>(matchState);
  case OperationType::Rename:
    return std::make_unique<RenameActionTab>(matchState, resources);
  case OperationType::Move:
  case OperationType::Copy:
    return std::make_unique<CopyMoveActionTab>(matchState, operation, resources);
  }

  assert(false);
  return nullptr;
}

ActionTab::ActionTab(pcontainer::ItemState const matchState) :
  matchState(matchState)
{
}

}} // namespace phobos::processwiz
