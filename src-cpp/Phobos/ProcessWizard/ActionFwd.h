#ifndef PROCESSWIZARD_ACTION_FWD_H
#define PROCESSWIZARD_ACTION_FWD_H

#include <memory>
#include <vector>

namespace phobos { namespace processwiz {

class Action;
using ConstAction = const Action;
using ConstActionPtr = std::shared_ptr<ConstAction>;
using ConstActionPtrVec = std::vector<ConstActionPtr>;

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_ACTION_FWD_H
