#ifndef WIDGETS_HV_LINE_H
#define WIDGETS_HV_LINE_H

#include <QFrame>

namespace phobos { namespace widgets {

class HVLine : public QFrame
{
public:
  HVLine(Qt::Orientation orientation, QWidget *parent = nullptr);
};

}} // namespace phobos::widgets

#endif // WIDGETS_HV_LLINE_H
