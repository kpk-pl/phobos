#ifndef IMAGEPROCESSING_NOISINESS_H
#define IMAGEPROCESSING_NOISINESS_H

#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess {

double noiseMeasure(cv::Mat const& cvImage, int medianSize);

}}

#endif // IMAGEPROCESSING_NOISINESS_H
