#ifndef WIDGETS_TOOLBAR_VERTICALGROUP_H
#define WIDGETS_TOOLBAR_VERTICALGROUP_H

#include <QWidget>

namespace phobos { namespace widgets { namespace toolbar {

class VerticalGroup : public QWidget
{
public:
  explicit VerticalGroup();

  template<typename... Widgets>
  static VerticalGroup* create(Widgets... widgets)
  {
    VerticalGroup *group = new VerticalGroup;
    auto unpack = {(group->addWidget(widgets), 0)...};
    return group;
  }

  void addWidget(QWidget *widget);
};

}}} // namespace phobos::widgets::toolbar

#endif // WIDGETS_TOOLBAR_VERTICALGROUP_H
