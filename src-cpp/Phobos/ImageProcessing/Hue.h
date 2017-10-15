#ifndef IMAGEPROCESSING_HUE_H
#define IMAGEPROCESSING_HUE_H

#include "ImageProcessing/Metrics.h"
#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess {

metric::Hue hueChannels(cv::Mat const& hsvImage);
double complementaryChannels(metric::Hue const& hue);

}}

#endif // IMAGEPROCESSING_HUE_H
