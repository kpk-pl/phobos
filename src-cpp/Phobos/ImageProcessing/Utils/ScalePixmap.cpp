#include "ImageProcessing/Utils/ScalePixmap.h"

namespace phobos { namespace iprocess { namespace utils {

QPixmap scalePixmap(QPixmap const& pixmap, QSize const& size)
{
  return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

}}} // namespace phobos::iprocess::utils
