#include <QLayout>
#include <QWidget>
#include "Utils/LayoutClear.h"

namespace phobos { namespace utils {

void clearLayoutItem(QLayoutItem* item, bool deleteWidgets)
{
  if (!item)
      return;

  QWidget* widget = item->widget();
  if (widget)
  {
    widget->setParent(nullptr);
    if (deleteWidgets)
      delete widget;
  }

  QLayout* insideLayout = item->layout();
  if (insideLayout)
  {
    clearLayout(insideLayout, deleteWidgets);
    insideLayout->setParent(nullptr);
    if (deleteWidgets)
      delete insideLayout;
  }

  delete item;
}

void clearLayoutItem(QLayout* layout, int index, bool deleteWidgets)
{
  auto item = layout->takeAt(index);
  if (item)
    clearLayoutItem(item, deleteWidgets);
}

void clearLayout(QLayout* layout, bool deleteWidgets)
{
  while(layout->count() > 0)
    clearLayoutItem(layout, 0, deleteWidgets);
}

}} // namespace phobos::utils
