#ifndef PHOTOCONTAINERS_ITEM_STATE_H
#define PHOTOCONTAINERS_ITEM_STATE_H

#include <string>
#include <iosfwd>

class QColor;

namespace phobos { namespace pcontainer {

enum class ItemState
{
  SELECTED,
  IGNORED
};

std::ostream& operator<<(std::ostream &ss, ItemState const& state);

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_ITEM_STATE_H
