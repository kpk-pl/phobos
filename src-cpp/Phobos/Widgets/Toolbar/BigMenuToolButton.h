#pragma once

#include "Widgets/Toolbar/BigToolButton.h"
#include <map>
#include <memory>

namespace phobos { namespace widgets { namespace toolbar {

class BigMenuToolButton : public BigToolButton
{
Q_OBJECT
public:
  explicit BigMenuToolButton(config::ConfigPath const& configPath);

  void addOption(QString const& label, std::string const& configName);

  Signal const* getSignal(std::string const& name) const override;

protected:
  std::map<std::string, std::unique_ptr<Signal>> _signalMap;
  config::ConfigPath const _baseConfig;

private slots:
  void changedOption(QString const& label, std::string const& configName);
  void clickedHandler();

private:
  void updateUi(QString const& label);

  std::string _currentSelection;
};

}}} // namespace phobos::widgets::toolbar
