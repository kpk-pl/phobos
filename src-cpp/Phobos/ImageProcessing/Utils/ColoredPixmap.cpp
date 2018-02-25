#include "ImageProcessing/Utils/ColoredPixmap.h"
#include "ConfigExtension.h"
#include <QPainter>

namespace phobos { namespace iprocess { namespace utils {

namespace {
  QPixmap getScaledPixmap(std::string const& fileName, QSize const& size)
  {
    QPixmap pixmap(fileName.c_str());
    if (pixmap.isNull())
        pixmap = QPixmap(size);

    if (size.isValid())
      pixmap = pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return pixmap;
  }
} // unnamed namespace

QPixmap coloredPixmap(std::string const& fileName, QColor const& color,
                      QSize const& size, double const opacity)
{
  QPixmap pixmap = getScaledPixmap(fileName, size);
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

// TODO: handle opacity
QPixmap coloredPixmap(std::string const& fileName, QSize const& size, double const)
{
  QPixmap pixmap = getScaledPixmap(fileName, size);
  return pixmap;
}

QPixmap coloredPixmap(config::ConfigPath const& path, QSize const& size, double const opacity)
{
  auto const iconPath = config::qualified<std::string>(path("icon"));
  if (!iconPath)
    return QPixmap();

  auto const color = config::qColor(path("color"));
  if (color)
    return coloredPixmap(*iconPath, *color, size, opacity);
  else
    return coloredPixmap(*iconPath, size, opacity);
}

}}} // namespace phobos::iprocess::utils
