#include "ImageProcessing/Utils/ColoredPixmap.h"
#include "ConfigExtension.h"
#include <QPainter>

namespace phobos { namespace iprocess { namespace utils {

QPixmap coloredPixmap(std::string const& fileName, QColor const& color,
                      QSize const& size, double const opacity)
{
    QPixmap pixmap(fileName.c_str());
    if (pixmap.isNull())
        pixmap = QPixmap(size);

    if (size.isValid())
      pixmap = pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainter painter(&pixmap);
    painter.setOpacity(opacity);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setBrush(color);
    painter.setPen(color);

    painter.drawRect(pixmap.rect());
    painter.end();

    return pixmap;
}

QPixmap coloredPixmap(config::ConfigPath const& path,
                      QSize const& size, double const opacity)
{
  std::string const iconPath = config::qualified(path("icon"), std::string{});
  QColor const color = config::qColor(path("color"), Qt::black);
  return coloredPixmap(iconPath, color, size, opacity);
}

}}} // namespace phobos::iprocess::utils
