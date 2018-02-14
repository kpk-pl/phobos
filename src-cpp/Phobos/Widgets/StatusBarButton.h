#ifndef STATUSBARBUTTON_H
#define STATUSBARBUTTON_H

#include "ConfigPath.h"
#include <QPushButton>

namespace phobos { namespace widgets {

class StatusBarButton : public QPushButton
{
public:
  explicit StatusBarButton(config::ConfigPath const& path);
};

}} // namespace phobos::widgets

#endif // STATUSBARBUTTON_H
