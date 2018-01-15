#include "Widgets/HVLine.h"

namespace phobos { namespace widgets {

HVLine::HVLine(Qt::Orientation orientation, QWidget *parent) :
  QFrame(parent)
{
  setFrameShadow(QFrame::Sunken);
  setContentsMargins(0, 0, 0, 0);

  switch(orientation) {
  case Qt::Vertical:
    setFrameShape(QFrame::VLine);
    break;
  case Qt::Horizontal:
    setFrameShape(QFrame::HLine);
    break;
  }
}

}} // namespace phobos::widgets
