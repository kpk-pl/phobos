#ifndef IMAGE_PROCESSING_COLOREDPIXMAP_H
#define IMAGE_PROCESSING_COLOREDPIXMAP_H

#include <string>
#include <QPixmap>
#include <QSize>

namespace phobos { namespace iprocess {

QPixmap coloredPixmap(std::string const& fileName, QSize const& size,
                      QColor const& color, double const opacity);

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_COLOREDPIXMAP_H
