#include "ImageProcessing/Utils/FormatConversion.h"

namespace phobos { namespace iprocess { namespace utils {

QImage convCvToImage(cv::Mat const& cvImage)
{
  QImage result((uchar*)cvImage.data, cvImage.cols, cvImage.rows, cvImage.step, QImage::Format_RGB888);
  return result.rgbSwapped();
}

}}} // namespace phobos::iprocess::utils
