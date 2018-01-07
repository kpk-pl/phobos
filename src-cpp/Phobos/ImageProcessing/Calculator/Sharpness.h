#ifndef IMAGE_PROCESSING_CALCULATOR_SHARPNESS_H
#define IMAGE_PROCESSING_CALCULATOR_SHARPNESS_H

#include "ImageProcessing/Metric/Sharpness.h"
#include "ImageProcessing/Metric/DepthOfField.h"
#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess { namespace calc {

namespace sharpness {
struct Homogeneous{};
struct Gaussian{};
} // namespace sharpness

// value of 1 means deep DoF, the bigger values the more shallow DoF
template<typename Strategy>
struct Sharpness
{
  Sharpness(cv::Mat const& cvImage, int kernel);

  metric::Sharpness sharpness() const;
  metric::DepthOfField depthOfField() const;

private:
  cv::Mat const highImage;
  double const percentile = 0.98;
};

}}} // namespace phobos::iprocess::calc

#endif // IMAGE_PROCESSING_CALCULATOR_SHARPNESS_H
