#include "Utils/ItemStateColor.h"
#include "Utils/Asserted.h"
#include "Config.h"
#include "ConfigExtension.h"

namespace phobos { namespace utils {

QColor itemStateColor(pcontainer::ItemState const& state)
{
  switch(state)
  {
  case pcontainer::ItemState::IGNORED:
    return config::qColor("category.ignored.color", Qt::lightGray);
  case pcontainer::ItemState::SELECTED:
    return config::qColor("category.selected.color", Qt::green);
  }

  return utils::asserted::always;
}

}} // namespace phobos::utils
