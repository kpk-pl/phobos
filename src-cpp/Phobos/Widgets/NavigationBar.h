#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>
#include <map>
#include <string>

class QPushButton;
class QHBoxLayout;

namespace phobos { namespace widgets {

class NavigationBar : public QWidget
{
public:
  explicit NavigationBar(QWidget *parent = nullptr);

  void setContentsMargins(int left, int top, int right, int bottom) const;

  QPushButton* addButton(std::string const& name);
  void addSeparator();
  void addStretch();

  QPushButton* button(std::string const& name) const;

  void showButtons();
  void hideButtons();

private slots:
  void showHideAction();

private:
  std::map<std::string, QPushButton*> _buttons;
  QHBoxLayout *_layout;

  QPushButton *_showHideButton;
  bool hidden;
};

}} // namespace phobos::widgets

#endif // NAVIGATIONBAR_H
