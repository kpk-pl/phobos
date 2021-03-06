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

namespace widgets { namespace toolbar {
class ToolButton;
class Signal;
namespace detail {
class HorizontalGroupBase;
} // namespace detail
}} // namespace widgets::toolbar

class MainToolbar : public QWidget
{
public:
  explicit MainToolbar(QWidget *parent = nullptr);

  void setContentsMargins(int left, int top, int right, int bottom) const;
  widgets::toolbar::Signal const* getSignal(std::string const& key) const;
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
  QWidget* setupEnhanceGroup();
  QWidget* setupHelpGroup();

  QWidget* addGroupWithSeparator(widgets::toolbar::detail::HorizontalGroupBase *group,
                                 std::string const& name,
                                 QHBoxLayout *layout);

  void setupHideButton(QBoxLayout *target);

  struct GroupInfo
  {
    QWidget* widget;
    bool visibility;
  };

  std::map<std::string, widgets::toolbar::Signal const*> _buttonSignals;
  std::map<std::string, GroupInfo> _groups;
  std::set<std::string> _groupNames;

  QHBoxLayout *_layout;
  QToolButton *_hideButton;
  std::size_t _fixedSizeHint;
  bool _hidden;
};

} // namespace phobos

#endif // MAINTOOLBAR_H
