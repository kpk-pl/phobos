#include "ImageProcessing/Utils/ColoredPixmap.h"
#include "Widgets/Toolbar/ToolButton.h"
#include "ConfigExtension.h"

namespace phobos { namespace widgets { namespace toolbar {

ToolButton::ToolButton(config::ConfigPath const& configPath)
{
  setIcon(iprocess::utils::coloredPixmap(configPath, QSize(64, 64)));
  setAutoRaise(true);
}

ToolButton::ToolButton(QString const& label, config::ConfigPath const& configPath) :
  ToolButton(configPath)
{
  setText(label);
}

QString ToolButton::property(QString const&) const
{
  return QString{};
}

}}} // namespace phobos::widgets::toolbar
