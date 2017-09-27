#include <QMouseEvent>
#include "Widgets/ClickableLabel.h"

namespace phobos { namespace widgets {

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags f) : QLabel(parent, f)
{}

ClickableLabel::ClickableLabel(QString const& text, QWidget *parent, Qt::WindowFlags f) :
  QLabel(text, parent, f)
{}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    emit clicked();
}

}} // namespace phobos::widgets
