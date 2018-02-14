#ifndef STATUSBARSLIDER_H
#define STATUSBARSLIDER_H

#include "ConfigPath.h"
#include <QWidget>

class QSlider;
class QPushButton;

namespace phobos { namespace widgets {

class StatusBarSlider : public QWidget
{
  Q_OBJECT
public:
  explicit StatusBarSlider(config::ConfigPath const& configPath);

signals:
  void valueChanged(int value);

private:
  QSlider *slider;
  QPushButton *minus, *plus;
};

}} // namespace phobos::widgets

#endif // STATUSBARSLIDER_H
