#ifndef IMAGEPROCESSING_CALCULATOR_HUE_H
#define IMAGEPROCESSING_CALCULATOR_HUE_H

#include "ImageProcessing/Feature/Hue.h"
#include "ImageProcessing/Metric/ComplementaryColors.h"
#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess { namespace calc {

feature::Hue hueChannels(cv::Mat const& hsvImage);
metric::ComplementaryColors complementaryChannels(feature::Hue const& hue);

}}} // namespace phobos::iprocess::calc

#endif // IMAGEPROCESSING_CALCULATOR_HUE_H
