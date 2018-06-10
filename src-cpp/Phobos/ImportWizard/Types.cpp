#include "ImportWizard/Types.h"

namespace phobos { namespace importwiz {

bool Photo::TimeNameComp::operator()(Photo const& lhs, Photo const& rhs) const
{
  if (lhs.info.timestamp != rhs.info.timestamp)
    return lhs.info.timestamp < rhs.info.timestamp;

  return lhs.name < rhs.name;
}

}} // namespace phobos::importwiz
