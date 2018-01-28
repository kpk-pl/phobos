#ifndef IMAGEPROCESSING_METRIC_SHARPNESS_H
#define IMAGEPROCESSING_METRIC_SHARPNESS_H

#include "ImageProcessing/Metric/MetricType.h"

namespace phobos { namespace iprocess { namespace metric {

namespace trait {
struct Sharpness
{
  static constexpr double minimum = 1.0;
  static constexpr double maximum = 128.0;

  using Comparing = comparing::BiggerIsBetter;
  using Aggregation = aggregation::Relative;
};
} // namespace trait

using Sharpness = MetricType<trait::Sharpness>;

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_SHARPNESS_H
