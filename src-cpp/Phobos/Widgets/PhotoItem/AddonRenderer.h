#ifndef WIDGETS_PHOTOITEM_ADDON_RENDERER_H
#define WIDGETS_PHOTOITEM_ADDON_RENDERER_H

#include "ConfigPath.h"
#include "ImageProcessing/Feature/Histogram.h"
#include "ImageProcessing/Feature/Hue.h"
#include <QPoint>

class QPainter;
class QSize;

namespace phobos { namespace iprocess { namespace metric {
class Histogram;
class Hue;
}}}

namespace phobos { namespace widgets { namespace pitem {

class AddonRenderer
{
public:
  AddonRenderer(QPainter &painter, config::ConfigPath const& baseConfigPath);

  void histogram(iprocess::feature::Histogram const& data,
                 QSize const& prefferedSize,
                 QPoint const& origin = QPoint(0,0));

  void cumulativeHistogram(iprocess::feature::Histogram const& data,
                           QSize const& prefferedSize,
                           QPoint const& origin = QPoint(0,0));

  void hueDisplay(iprocess::feature::Hue const& data,
                  QSize const& prefferedSize,
                  QPoint const& origin = QPoint(0,0));

private:
  void drawHistLike(std::vector<float> const& data,
                    config::ConfigPath const& configPath,
                    QPoint const& origin,
                    QSize const& size);

  QPainter &painter;
  config::ConfigPath const baseConfigPath;
};

}}} // namespace phobos::widgets::pitem

#endif // WIDGETS_PHOTOITEM_ADDON_RENDERER_H
