#ifndef WELCOMEVIEW_H
#define WELCOMEVIEW_H

#include "Views/Screen.h"

namespace phobos { namespace view {

class Welcome : public Screen
{
  Q_OBJECT
public:
  explicit Welcome();
};

}} // namespace phobos::view

#endif // WELCOMEVIEW_H
