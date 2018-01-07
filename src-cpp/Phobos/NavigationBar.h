#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>

namespace phobos {

class NavigationBar : public QWidget
{
public:
  explicit NavigationBar(bool includeSlider);

  void setContentsMargins(int left, int top, int right, int bottom) const;

  QPushButton *leftButton, *rightButton;
  QPushButton *prevSeriesButton, *nextSeriesButton;
  QPushButton *allSeriesButton, *numSeriesButton, *oneSeriesButton;

  QSlider* slider() const { return _slider; }

private:
  QSlider *_slider;
};

} // namespace phobos

#endif // NAVIGATIONBAR_H
