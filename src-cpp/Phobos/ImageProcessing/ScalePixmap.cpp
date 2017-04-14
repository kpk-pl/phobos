#include "ImageProcessing/ScalePixmap.h"

namespace phobos { namespace iprocess {

QPixmap scalePixmap(QPixmap const& pixmap, QSize const& size)
{
    return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

}} // namespace phobos::iprocess
