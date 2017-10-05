#ifndef WIDGETS_PHOTOITEM_ADDON_RENDERER_H
#define WIDGETS_PHOTOITEM_ADDON_RENDERER_H

#include <QPoint>

class QPainter;
class QSize;

namespace phobos { namespace iprocess {
class Histogram;
}}

namespace phobos { namespace widgets { namespace pitem {

class AddonRenderer
{
public:
  AddonRenderer(QPainter &painter);

  void histogram(iprocess::Histogram const& data,
                 QSize const& prefferedSize,
                 QPoint const& origin = QPoint(0,0));

private:
  QPainter &painter;
};

}}} // namespace phobos::widgets::pitem

#endif // WIDGETS_PHOTOITEM_ADDON_RENDERER_H
