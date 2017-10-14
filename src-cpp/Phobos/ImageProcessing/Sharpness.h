#ifndef IMAGE_PROCESSING_SHARPNESS_H
#define IMAGE_PROCESSING_SHARPNESS_H

#include <opencv2/core/core.hpp>
#include <tuple>

namespace phobos { namespace iprocess { namespace sharpness {

struct Result {
  double sharpness;
  std::tuple<double, double, double> breakout;
};

Result homogeneous(cv::Mat const& cvImage, int const kernel);
Result gaussian(cv::Mat const& cvImage, int const kernel);

// value of 1 means deep DoF, the bigger values the more shallow DoF
double depthOfField(Result const& res);

}}} // namespace phobos::iprocess::sharpness

#endif // IMAGE_PROCESSING_SHARPNESS_H
