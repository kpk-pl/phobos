#include "PhotoContainers/ItemState.h"
#include "Utils/Asserted.h"
#include <ostream>
#include <map>

namespace phobos { namespace pcontainer {

namespace {
  static std::map<ItemState, std::string> const stateToString = {
    { ItemState::SELECTED, "selected" },
    { ItemState::UNKNOWN, "unknown" }
  };
} // unnamed namespace

std::ostream& operator<<(std::ostream &ss, ItemState const& state)
{
  ss << utils::asserted::fromMap(stateToString, state);
  return ss;
}

}} // namespace phobos::pcontainer
