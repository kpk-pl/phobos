#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include <QWidget>
#include <map>

class QToolButton;
class QHBoxLayout;
class QBoxLayout;

namespace phobos {

class MainToolbar : public QWidget
{
public:
  explicit MainToolbar(QWidget *parent = nullptr);

  void setContentsMargins(int left, int top, int right, int bottom) const;
  QToolButton* getButton(std::string const& key) const;

public slots:
  void setHidden(bool hide);

private:
  void addSeparator();

  void setupFileGroup();
  void setupViewGroup();
  void setupSelectGroup();
  void setupProcessGroup();
  void setupHelpGroup();

  void setupHideButton(QBoxLayout *target);

  std::map<std::string, QToolButton*> _buttons;
  QToolButton* registerButton(std::string const& key, QToolButton *button);

  QHBoxLayout *_layout;
  QToolButton *_hideButton;
  bool _hidden;
};

} // namespace phobos

#endif // MAINTOOLBAR_H
