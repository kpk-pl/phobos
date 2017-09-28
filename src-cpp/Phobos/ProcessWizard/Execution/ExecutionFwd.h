#ifndef PROCESSWIZARD_EXECUTIONFWD_H
#define PROCESSWIZARD_EXECUTIONFWD_H

#include <memory>
#include <vector>

namespace phobos { namespace processwiz {

class Execution;
using ConstExecutionPtr = std::shared_ptr<const Execution>;
using ConstExecutionPtrVec = std::vector<ConstExecutionPtr>;

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_EXECUTIONFWD_H
