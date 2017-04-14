#ifndef IMAGE_PROCESSING_BLURINESS_H
#define IMAGE_PROCESSING_BLURINESS_H

#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess { namespace blur {

/*
 * Adapted from:
 * http://stackoverflow.com/questions/7765810/is-there-a-way-to-detect-if-an-image-is-blurry/7768918#7768918
 */

/*
 * Returns smaller values for blurry images
 * depth of cv.CV_32F allocates a lot of memory
 */
double sobel(cv::Mat const& cvImage, int depth = -1);

/*
 * Returns smaller values for blurry images
 */
double laplace(cv::Mat const& cvImage, int depth = -1);

/*
 * Returns smaller values for blurry images
 */
double laplaceMod(cv::Mat const& cvImage, int depth = -1);

}}} // namespace phobos::iprocess::blur

#endif // IMAGE_PROCESSING_BLURINESS_H
