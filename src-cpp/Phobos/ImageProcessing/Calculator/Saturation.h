#ifndef IMAGEPROCESSING_CALCULATOR_SATURATION_H
#define IMAGEPROCESSING_CALCULATOR_SATURATION_H

#include "ImageProcessing/Metric/Saturation.h"
#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess { namespace calc {

metric::Saturation saturation(cv::Mat const& hsvImage);

}}} // namespace phobos::iprocess::calc

#endif // IMAGEPROCESSING_CALCULATOR_SATURATION_H
