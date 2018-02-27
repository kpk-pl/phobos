#ifndef WIDGETS_TOOLBAR_INLINETOOLBUTTON_H
#define WIDGETS_TOOLBAR_INLINETOOLBUTTON_H

#include "Widgets/Toolbar/ToolButton.h"

namespace phobos { namespace widgets { namespace toolbar {

class InlineToolButton : public ToolButton
{
public:
  explicit InlineToolButton(QString const& label, config::ConfigPath const& configPath);
  explicit InlineToolButton(config::ConfigPath const& configPath);
};

}}} // namespace phobos::widgets::toolbar

#endif // WIDGETS_TOOLBAR_INLINETOOLBUTTON_H
