#include "Widgets/Toolbar/BigMenuToolButton.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"
#include "Utils/Asserted.h"
#include <QMenu>
#include <tuple>

namespace phobos { namespace widgets { namespace toolbar {

BigMenuToolButton::BigMenuToolButton(config::ConfigPath const& configPath) :
  BigToolButton("", configPath),
  _baseConfig(configPath)
{
  QMenu *optionsMenu = new QMenu(this);

  setMenu(optionsMenu);
  setPopupMode(QToolButton::MenuButtonPopup);
}

void BigMenuToolButton::addOption(QString const& label, std::string const& configName)
{
  _signalMap.emplace(configName, std::make_unique<Signal>());

  menu()->addAction(label, [=](){ changedOption(label, configName); });

  if (_currentSelection.empty()) // first item
  {
    _currentSelection = configName;
    updateUi(label);
  }
}

void BigMenuToolButton::updateUi(QString const& label)
{
  setIcon(iprocess::utils::coloredPixmap(_baseConfig(_currentSelection), QSize(64, 64)));
  setText(label);
}

void BigMenuToolButton::changedOption(QString const& label, std::string const& configName)
{
  _currentSelection = configName;
  updateUi(label);

  clickedHandler();
}

void BigMenuToolButton::clickedHandler()
{
  if (_currentSelection.empty())
    return;
  emit utils::asserted::fromMap(_signalMap, _currentSelection)->activated();
}

Signal const* BigMenuToolButton::getSignal(std::string const& name) const
{
  auto const it = _signalMap.find(name);
  if (it != _signalMap.end())
    return it->second.get();
  return BigToolButton::getSignal(name);
}

}}} // namespace phobos::widgets::toolbar
