#ifndef IMAGE_PROCESSING_SHARPNESS_H
#define IMAGE_PROCESSING_SHARPNESS_H

#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess { namespace sharpness {

double homogeneous(cv::Mat const& cvImage, int const kernel);
double gaussian(cv::Mat const& cvImage, int const kernel);

}}} // namespace phobos::iprocess::sharpness

#endif // IMAGE_PROCESSING_SHARPNESS_H
