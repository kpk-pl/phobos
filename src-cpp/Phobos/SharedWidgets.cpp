#include "SharedWidgets.h"
#include "Widgets/StatusBarLeftRightNavigation.h"
#include "Widgets/StatusBarSlider.h"

namespace phobos {

namespace {
config::ConfigPath const configPath("statusBar");
} // unnamed namespace

SharedWidgets::SharedWidgets()
{
  slider = new widgets::StatusBarSlider(configPath("slider"));
  leftRightNav = new widgets::StatusBarLeftRightNavigation(configPath("leftRightNav"));
}

} // namespace phobos
