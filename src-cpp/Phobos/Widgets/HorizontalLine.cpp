#include "Widgets/HorizontalLine.h"

namespace phobos { namespace widgets {

HorizontalLine::HorizontalLine(QWidget *parent) :
  QFrame(parent)
{
  this->setFrameShape(QFrame::HLine);
  this->setFrameShadow(QFrame::Sunken);
}

}} // namespace phobos::widgets
