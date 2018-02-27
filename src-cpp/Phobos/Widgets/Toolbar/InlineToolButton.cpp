#include "Widgets/Toolbar/InlineToolButton.h"
#include "ConfigExtension.h"

namespace phobos { namespace widgets { namespace toolbar {
namespace {
config::ConfigPath const basePath("navigationBar.config");
} // unnamed namespace

InlineToolButton::InlineToolButton(QString const& label, config::ConfigPath const& configPath) :
  ToolButton(label, configPath)
{
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  setIconSize(config::qSize(basePath("inlineButtonSize"), QSize(14, 14)));
  setContentsMargins(0, 0, 0, 0);
}

InlineToolButton::InlineToolButton(config::ConfigPath const& configPath) :
  ToolButton(configPath)
{
  setToolButtonStyle(Qt::ToolButtonIconOnly);
  setIconSize(config::qSize(basePath("inlineButtonSize"), QSize(14, 14)));
  setContentsMargins(0, 0, 0, 0);
}

}}} // namespace phobos::widgets::toolbar
