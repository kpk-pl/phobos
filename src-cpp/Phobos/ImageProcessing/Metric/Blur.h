#ifndef IMAGEPROCESSING_METRIC_BLUR_H
#define IMAGEPROCESSING_METRIC_BLUR_H

#include "ImageProcessing/Metric/MetricType.h"

namespace phobos { namespace iprocess { namespace metric {

namespace trait {
struct Blur
{
  static constexpr double minimum = 0.0;
  static constexpr double maximum = 16384.0;

  using Comparing = comparing::BiggerIsBetter;
  using Aggregation = aggregation::Relative;
};
} // namespace trait

using Blur = MetricType<trait::Blur>;

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_BLUR_H
