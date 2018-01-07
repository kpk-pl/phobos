#ifndef IMAGEPROCESSING_UTILS_COLORED_PIXMAP_H
#define IMAGEPROCESSING_UTILS_COLORED_PIXMAP_H

#include <string>
#include <QPixmap>
#include <QSize>

namespace phobos { namespace iprocess { namespace utils {

QPixmap coloredPixmap(std::string const& fileName, QColor const& color,
                      QSize const& size = QSize(), double const opacity = 1.0);

}}} // namespace phobos::iprocess::utils

#endif // IMAGEPROCESSING_UTILS_COLORED_PIXMAP_H
