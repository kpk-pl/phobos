#ifndef WIDGETS_PHOTOITEM_CAPABILITY_H
#define WIDGETS_PHOTOITEM_CAPABILITY_H

#include <set>

namespace phobos { namespace widgets { namespace pitem {

enum class CapabilityType
{
  OPEN_SERIES,
  REMOVE_PHOTO
};

struct Capabilities
{
  Capabilities() = default;
  Capabilities(CapabilityType type)
  {
    capabilities.insert(type);
  }

  friend Capabilities operator|(CapabilityType lhs, CapabilityType rhs);

  friend Capabilities operator|(Capabilities &&lhs, CapabilityType rhs)
  {
    Capabilities result(std::move(lhs));
    result.capabilities.insert(rhs);
    return result;
  }

  friend Capabilities operator|(CapabilityType lhs, Capabilities &&rhs)
  {
    return operator|(std::move(rhs), lhs);
  }

  bool has(CapabilityType const type) const
  {
    return capabilities.find(type) != capabilities.end();
  }

private:
  std::set<CapabilityType> capabilities;
};

inline Capabilities operator|(CapabilityType lhs, CapabilityType rhs)
{
  Capabilities c(lhs);
  c.capabilities.insert(rhs);
  return c;
}

}}} // namespace phobos::widgets::pitem

#endif // WIDGETS_PHOTOITEM_CAPABILITY_H
