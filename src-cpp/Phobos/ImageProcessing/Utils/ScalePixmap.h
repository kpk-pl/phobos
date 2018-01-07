#ifndef IMAGEPROCESSING_UTILS_SCALE_PIXMAP_H
#define IMAGEPROCESSING_UTILS_SCALE_PIXMAP_H

#include <QPixmap>
#include <QSize>

namespace phobos { namespace iprocess { namespace utils {

QPixmap scalePixmap(QPixmap const& pixmap, QSize const& size);

}}} // namespace phobos::iprocess::utils

#endif // IMAGEPROCESSING_UTILS_SCALE_PIXMAP_H
