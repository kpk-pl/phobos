#ifndef UTILS_PAINTERFRAME_H
#define UTILS_PAINTERFRAME_H

#include <QPainter>

namespace phobos { namespace utils {

struct PainterFrame
{
  PainterFrame(QPainter &painter) : _painter(painter)
  {
    _painter.save();
  }

  ~PainterFrame()
  {
    _painter.restore();
  }

private:
  QPainter& _painter;
};

}} // namespace phobos::utils

#endif // UTILS_PAINTERFRAME_H
