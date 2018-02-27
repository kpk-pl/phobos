#include "Widgets/Toolbar/BigToolButton.h"
#include "ConfigExtension.h"

namespace phobos { namespace widgets { namespace toolbar {
namespace {
config::ConfigPath const basePath("navigationBar.config");
} // unnamed namespace

BigToolButton::BigToolButton(QString const& label, config::ConfigPath const& configPath) :
  ToolButton(label, configPath)
{
  setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  setIconSize(config::qSize(basePath("config")("buttonSize"), QSize(40, 40)));
  setContentsMargins(0, 0, 0, 0);
}

}}} // namespace phobos::widgets::toolbar
