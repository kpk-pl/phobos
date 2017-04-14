#include "ImageProcessing/FormatConversion.h"

namespace phobos { namespace iprocess {

QImage convCvToImage(cv::Mat const& cvImage)
{
    QImage result((uchar*)cvImage.data, cvImage.cols, cvImage.rows, cvImage.step, QImage::Format_RGB888);
    return result.rgbSwapped();
}

}} // namespace phobos::iprocess
