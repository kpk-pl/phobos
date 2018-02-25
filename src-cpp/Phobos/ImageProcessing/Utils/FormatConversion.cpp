#include "ImageProcessing/Utils/FormatConversion.h"
#include "Utils/Asserted.h"

// from https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
namespace phobos { namespace iprocess { namespace utils {

QImage convCvToImage(cv::Mat const& cvImage)
{
  QImage result((uchar*)cvImage.data, cvImage.cols, cvImage.rows, cvImage.step, QImage::Format_RGB888);
  return result.rgbSwapped();
}

cv::Mat convImageToCv(QImage const& qImage, bool clone)
{
  switch(qImage.format())
  {
  // 8-bit, 4 channel
  case QImage::Format_ARGB32:
  case QImage::Format_ARGB32_Premultiplied:
  {
    cv::Mat mat(qImage.height(), qImage.width(), CV_8UC4, const_cast<uchar*>(qImage.bits()), static_cast<std::size_t>(qImage.bytesPerLine()));
    return clone ? mat.clone() : mat;
  }

  // 8-bit, 3 channel
  case QImage::Format_RGB32:
  case QImage::Format_RGB888:
  {
    QImage swapped = qImage;
    if (qImage.format() == QImage::Format_RGB32)
      swapped = swapped.convertToFormat(QImage::Format_RGB888);

    swapped = swapped.rgbSwapped();
    return cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), static_cast<size_t>(swapped.bytesPerLine())).clone();
  }

  // 8-bit, 1 channel
  case QImage::Format_Indexed8:
  {
    cv::Mat mat(qImage.height(), qImage.width(), CV_8UC1, const_cast<uchar*>(qImage.bits()), static_cast<size_t>(qImage.bytesPerLine()));
    return clone ? mat.clone() : mat;
  }

  default:
    break;
  }

  return phobos::utils::asserted::always;
}

}}} // namespace phobos::iprocess::utils
