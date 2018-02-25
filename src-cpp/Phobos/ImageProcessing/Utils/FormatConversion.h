#ifndef IMAGEPROCESSING_UTILS_FORMAT_CONVERSION_H
#define IMAGEPROCESSING_UTILS_FORMAT_CONVERSION_H

#include <opencv2/core/core.hpp>
#include <QImage>

namespace phobos { namespace iprocess { namespace utils {

QImage convCvToImage(cv::Mat const& cvImage);
cv::Mat convImageToCv(QImage const& qImage, bool clone = true);

}}} // namespace phobos::iprocess::utils

#endif // IMAGE_PROCESSING_UTILS_FORMAT_CONVERSION_H
