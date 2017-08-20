#include <QPainter>
#include "ImageProcessing/ColoredPixmap.h"

namespace phobos { namespace iprocess {

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

}} // namespace phobos::iprocess
