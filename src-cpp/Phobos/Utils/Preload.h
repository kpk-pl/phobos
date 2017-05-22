#ifndef PHOBOS_UTILS_PRELOAD_H_
#define PHOBOS_UTILS_PRELOAD_H_

#include <QSize>
#include <QPixmap>
#include <QImage>

namespace phobos { namespace utils {

inline QImage preloadImage(QSize const& size)
{
    QPixmap pixmap(size);
    pixmap.fill(Qt::lightGray);
    return pixmap.toImage();
}

}} // namespace phobos::utils

#endif // PHOBOS_UTILS_PRELOAD_H_
