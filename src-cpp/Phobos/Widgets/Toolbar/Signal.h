#ifndef WIDGETS_TOOLBAR_SIGNAL_H
#define WIDGETS_TOOLBAR_SIGNAL_H

#include <QObject>

namespace phobos { namespace widgets { namespace toolbar {

class Signal : public QObject
{
Q_OBJECT

signals:
  void activated() const;
};

}}} // namespace phobos::widgets::toolbar

#endif // WIDGETS_TOOLBAR_SIGNAL_H
