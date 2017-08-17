#ifndef PROCESSWIZARD_OPERATION_H
#define PROCESSWIZARD_OPERATION_H

#include <string>
#include <iosfwd>

namespace phobos { namespace processwiz {

enum class OperationType
{
  Delete, Copy, Move, Rename
};

std::ostream& operator<<(std::ostream &ss, OperationType const& operation);

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_OPERATION_H
