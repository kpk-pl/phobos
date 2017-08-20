#ifndef IMAGE_PROCESSING_COLOREDPIXMAP_H
#define IMAGE_PROCESSING_COLOREDPIXMAP_H

#include <string>
#include <QPixmap>
#include <QSize>

namespace phobos { namespace iprocess {

QPixmap coloredPixmap(std::string const& fileName, QColor const& color,
                      QSize const& size = QSize(), double const opacity = 1.0);

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_COLOREDPIXMAP_H
