#ifndef IMAGE_PROCESSING_HISTOGRAM_H
#define IMAGE_PROCESSING_HISTOGRAM_H

#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess {

std::vector<float> normalizedHistogram(cv::Mat const& cvImage, double &outContrast);

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_HISTOGRAM_H
