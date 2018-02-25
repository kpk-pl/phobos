#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include <QWidget>
#include <map>
#include <set>

class QToolButton;
class QHBoxLayout;
class QWidget;
class QBoxLayout;

namespace phobos {

class MainToolbar : public QWidget
{
public:
  explicit MainToolbar(QWidget *parent = nullptr);

  void setContentsMargins(int left, int top, int right, int bottom) const;
  QToolButton* getButton(std::string const& key) const;
  std::set<std::string> const& buttonGroups() const { return _groupNames; }

public slots:
  void setHidden(bool hide);
  void setGroupVisible(std::string const& group, bool visible);

private:
  QWidget* setupFileGroup();
  QWidget* setupViewGroup();
  QWidget* setupSeriesGroup();
  QWidget* setupSelectGroup();
  QWidget* setupProcessGroup();
  QWidget* setupHelpGroup();

  void setupHideButton(QBoxLayout *target);

  std::map<std::string, QToolButton*> _buttons;
  std::map<std::string, QWidget*> _groups;
  std::set<std::string> _groupNames;
  QToolButton* registerButton(std::string const& key, QToolButton *button);

  QHBoxLayout *_layout;
  QToolButton *_hideButton;
  bool _hidden;
};

} // namespace phobos

#endif // MAINTOOLBAR_H
