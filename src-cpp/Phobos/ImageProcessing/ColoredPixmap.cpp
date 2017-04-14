#include "ImageProcessing/ColoredPixmap.h"

#include <QPainter>

namespace phobos { namespace iprocess {

QPixmap coloredPixmap(std::string const& fileName, QSize const& size,
                      QColor const& color, double const opacity)
{
    QPixmap pixmap(fileName.c_str());
    if (pixmap.isNull())
        pixmap = QPixmap(size);
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
