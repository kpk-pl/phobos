#ifndef SHAREDWIDGETS_H
#define SHAREDWIDGETS_H

namespace phobos {

namespace widgets {
class StatusBarSlider;
class StatusBarLeftRightNavigation;
} // namespace widgets

struct SharedWidgets
{
  explicit SharedWidgets();

  widgets::StatusBarSlider *slider;
  widgets::StatusBarLeftRightNavigation *leftRightNav;
};

} // namespace phobos

#endif // SHAREDWIDGETS_H
