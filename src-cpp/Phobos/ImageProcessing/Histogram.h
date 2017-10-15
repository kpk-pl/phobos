#ifndef IMAGE_PROCESSING_HISTOGRAM_H
#define IMAGE_PROCESSING_HISTOGRAM_H

#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess {

cv::Mat grayscaleHistogram(cv::Mat const& cvImage);
std::vector<float> normalizedHistogram(cv::Mat const& cvImage, double *outContrast);

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_HISTOGRAM_H
