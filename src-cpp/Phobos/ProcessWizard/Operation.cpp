#include "ProcessWizard/Operation.h"
#include "Utils/Asserted.h"
#include <map>
#include <ostream>

namespace phobos { namespace processwiz {

namespace {
  static std::map<OperationType, std::string> const operationToString = {
    {OperationType::Delete, "delete"},
    {OperationType::Copy, "copy"},
    {OperationType::Move, "move"},
    {OperationType::Rename, "rename"}
  };
} // unnamed namespace

std::ostream& operator<<(std::ostream &ss, OperationType const& operation)
{
  ss << utils::asserted::fromMap(operationToString, operation);
  return ss;
}

}} // namespace phobos::processwiz
