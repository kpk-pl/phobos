#ifndef PROCESSWIZARD_EXECUTION_EXECUTE_H
#define PROCESSWIZARD_EXECUTION_EXECUTE_H

#include "ProcessWizard/Execution/ExecutionFwd.h"
#include "PhotoContainers/Fwd.h"

namespace phobos { namespace processwiz { namespace exec {

void execute(pcontainer::Set &photoSet, ConstExecutionPtrVec const& executions);

}}} // namespace phobos::processwiz::exec

#endif // PROCESSWIZARD_EXECUTION_EXECUTE_H
