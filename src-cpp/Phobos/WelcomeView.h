#ifndef WELCOMEVIEW_H
#define WELCOMEVIEW_H

#include <QWidget>

namespace phobos {

class WelcomeView : public QWidget
{
  Q_OBJECT
public:
  explicit WelcomeView(QWidget* parent = nullptr);
};

} // namespace phobos

#endif // WELCOMEVIEW_H
