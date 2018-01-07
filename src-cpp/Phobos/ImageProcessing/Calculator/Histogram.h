#ifndef IMAGEPROCESSING_CALCULATOR_HISTOGRAM_H
#define IMAGEPROCESSING_CALCULATOR_HISTOGRAM_H

#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess { namespace calc {

cv::Mat grayscaleHistogram(cv::Mat const& cvImage);
std::vector<float> normalizedHistogram(cv::Mat const& cvImage, double *outContrast);

}}} // namespace phobos::iprocess::calc

#endif // IMAGEPROCESSING_CALCULATOR_HISTOGRAM_H
