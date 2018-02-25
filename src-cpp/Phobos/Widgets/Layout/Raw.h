#ifndef WIDGETS_RAWLAYOUT_H
#define WIDGETS_RAWLAYOUT_H

class QWidget;

namespace phobos { namespace widgets { namespace layout {

template<typename T>
T* makeRaw(QWidget *parent = nullptr)
{
  T* layout = new T(parent);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  return layout;
}

}}} // namespace phobos::widgets::layout

#endif // WIDGETS_RAWLAYOUT_H
