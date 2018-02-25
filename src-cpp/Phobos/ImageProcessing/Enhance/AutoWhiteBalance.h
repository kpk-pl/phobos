#ifndef IMAGEPROCESSING_ENHANCE_AUTOWHITEBALANCE_H
#define IMAGEPROCESSING_ENHANCE_AUTOWHITEBALANCE_H

#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess { namespace enhance {

cv::Mat autoWhiteBalance(cv::Mat const& source);

}}} // namespace phobos::iprocess::enhance

#endif // IMAGEPROCESSING_ENHANCE_AUTOWHITEBALANCE_H
