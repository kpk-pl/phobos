#ifndef LEFTRIGHTNAVIGATION_H
#define LEFTRIGHTNAVIGATION_H

#include "ConfigPath.h"
#include <QWidget>

class QPushButton;

namespace phobos { namespace widgets {

class StatusBarLeftRightNavigation : public QWidget
{
  Q_OBJECT
public:
  explicit StatusBarLeftRightNavigation(config::ConfigPath const& configPath);

signals:
  void leftClicked();
  void rightClicked();

private:
  QPushButton *leftButton, *rightButton;
};

}} // namespace phobos::widgets

#endif // LEFTRIGHTNAVIGATION_H
