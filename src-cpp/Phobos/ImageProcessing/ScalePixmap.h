#ifndef IMAGE_PROCESSING_SCALEPIXMAP_H
#define IMAGE_PROCESSING_SCALEPIXMAP_H

#include <QPixmap>
#include <QSize>

namespace phobos { namespace iprocess {

QPixmap scalePixmap(QPixmap const& pixmap, QSize const& size);

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_SCALEPIXMAP_H
