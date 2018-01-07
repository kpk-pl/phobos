#ifndef IMAGEPROCESSING_CALCULATOR_NOISINESS_H
#define IMAGEPROCESSING_CALCULATOR_NOISINESS_H

#include "ImageProcessing/Metric/Noise.h"
#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess { namespace calc {

/*
 * Noise estimate is the difference between an image and median filtered version of the image.
 * The algorithm is almost exactly the same as for sharpness, but the metric os somewhat difficult
 * to interpret. Low noise values indicate that an image is blurred, but high values indicate an
 * image has a lot of noise.
 */
metric::Noise noiseMeasure(cv::Mat const& cvImage, int medianSize);

}}}

#endif // IMAGEPROCESSING_CALCULATOR_NOISINESS_H
