#ifndef WIDGETS_TOOLBAR_HORIZONTALGROUP_H
#define WIDGETS_TOOLBAR_HORIZONTALGROUP_H

#include <QWidget>
#include <QString>

class QHBoxLayout;

namespace phobos { namespace widgets { namespace toolbar {

namespace detail {
class HorizontalGroupBase : public QWidget
{
public:
  void addWidget(QWidget *widget);

  void setVisible(bool visible) override;
  void setVisibleBuddy(QWidget *buddy);

protected:
  explicit HorizontalGroupBase();

  QHBoxLayout* _buttonLayout;

private:
  QWidget* _buddy;
};
} // namespace detail

class HorizontalGroup : public detail::HorizontalGroupBase
{
public:
  explicit HorizontalGroup();

  template<typename... Widgets>
  static HorizontalGroup* create(Widgets... widgets)
  {
    HorizontalGroup* group = new HorizontalGroup();
    auto unpack = {(group->addWidget(widgets), 0)...};
    return group;
  }
};

class NamedHorizontalGroup : public detail::HorizontalGroupBase
{
public:
  explicit NamedHorizontalGroup(QString const& name);

  template<typename... Widgets>
  static NamedHorizontalGroup* create(QString const& name, Widgets... widgets)
  {
    NamedHorizontalGroup* group = new NamedHorizontalGroup(name);
    auto unpack = {(group->addWidget(widgets), 0)...};
    return group;
  }
};

}}} // namespace phobos::widgets::toolbar

#endif // WIDGETS_TOOLBAR_HORIZONTALGROUP_H
