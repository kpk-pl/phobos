#ifndef WIDGETS_TOOLBAR_BIGTOOLBUTTON_H
#define WIDGETS_TOOLBAR_BIGTOOLBUTTON_H

#include "Widgets/Toolbar/ToolButton.h"

namespace phobos { namespace widgets { namespace toolbar {

class BigToolButton : public ToolButton
{
public:
  explicit BigToolButton(QString const& label, config::ConfigPath const& configPath);
};

}}} // namespace phobos::widgets::toolbar

#endif // WIDGETS_TOOLBAR_BIGTOOLBUTTON_H
