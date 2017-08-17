#include "Utils/ItemStateColor.h"
#include "Config.h"
#include "ConfigExtension.h"
#include <cassert>

namespace phobos { namespace utils {

QColor itemStateColor(pcontainer::ItemState const& state)
{
  switch(state)
  {
  case pcontainer::ItemState::UNKNOWN:
    return config::qColor("photoItemWidget.border.unknown", Qt::lightGray);
  case pcontainer::ItemState::SELECTED:
    return config::qColor("photoItemWidget.border.selected", Qt::green);
  case pcontainer::ItemState::DISCARDED:
    return config::qColor("photoItemWidget.border.discarded", Qt::red);
  }

  assert(false);
  return QColor();
}

}} // namespace phobos::utils
