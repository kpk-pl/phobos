#include "ImageProcessing/Utils/ColoredPixmap.h"
#include "Widgets/Toolbar/ToolButton.h"
#include "ConfigExtension.h"

namespace phobos { namespace widgets { namespace toolbar {

ToolButton::ToolButton(config::ConfigPath const& configPath)
{
  setIcon(iprocess::utils::coloredPixmap(configPath, QSize(64, 64)));
  setAutoRaise(true);

  QObject::connect(this, &ToolButton::clicked, &_clickedSignal, &Signal::activated);
}

ToolButton::ToolButton(QString const& label, config::ConfigPath const& configPath) :
  ToolButton(configPath)
{
  setText(label);
}

Signal const* ToolButton::getSignal(std::string const& name) const
{
  return name == "clicked" ? &_clickedSignal : nullptr;
}

}}} // namespace phobos::widgets::toolbar
