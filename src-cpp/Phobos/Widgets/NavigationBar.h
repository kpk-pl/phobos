#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>
#include <map>
#include <string>

class QSlider;
class QPushButton;
class QHBoxLayout;

namespace phobos { namespace widgets {

class NavigationBar : public QWidget
{
public:
  explicit NavigationBar();

  void setContentsMargins(int left, int top, int right, int bottom) const;

  QPushButton* addButton(std::string const& name, std::string const& icon);
  QSlider* addSlider();
  void addSeparator();
  void addStretch();

  QPushButton* button(std::string const& name) const;
  QSlider* slider() const { return _slider; }

private:
  std::map<std::string, QPushButton*> _buttons;
  QSlider *_slider;
  QHBoxLayout *_layout;
};

}} // namespace phobos::widgets

#endif // NAVIGATIONBAR_H
