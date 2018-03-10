#ifndef WIDGETS_TOOLBAR_TOOLBUTTON_H
#define WIDGETS_TOOLBAR_TOOLBUTTON_H

#include "ConfigPath.h"
#include "Widgets/Toolbar/Signal.h"
#include <QToolButton>
#include <QString>

namespace phobos { namespace widgets { namespace toolbar {

class ToolButton : public QToolButton
{
public:
  explicit ToolButton(config::ConfigPath const& configPath);
  explicit ToolButton(QString const& label, config::ConfigPath const& configPath);

  virtual Signal const* getSignal(std::string const& name) const;

protected:
  Signal const _clickedSignal;
};

}}} // namespace phobos::widgets::toolbar

#endif // WIDGETS_TOOLBAR_TOOLBUTTON_H
