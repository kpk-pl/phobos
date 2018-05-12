#ifndef PHOTOCONTAINERS_ITEM_STATE_H
#define PHOTOCONTAINERS_ITEM_STATE_H

#include <string>
#include <iosfwd>

class QColor;

namespace phobos { namespace pcontainer {

// TODO: Rename UNKNOWN to UNSELECTED
// Update logs and Processing Wizard texts for that
enum class ItemState
{
  SELECTED,
  UNKNOWN
};

std::ostream& operator<<(std::ostream &ss, ItemState const& state);

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_ITEM_STATE_H
