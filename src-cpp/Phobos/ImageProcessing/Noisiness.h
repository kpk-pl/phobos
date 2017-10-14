#ifndef IMAGEPROCESSING_NOISINESS_H
#define IMAGEPROCESSING_NOISINESS_H

#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess {

/*
 * Noise estimate is the difference between an image and median filtered version of the image.
 * The algorithm is almost exactly the same as for sharpness, but the metric os somewhat difficult
 * to interpret. Low noise values indicate that an image is blurred, but high values indicate an
 * image has a lot of noise.
 */
double noiseMeasure(cv::Mat const& cvImage, int medianSize);

}}

#endif // IMAGEPROCESSING_NOISINESS_H
