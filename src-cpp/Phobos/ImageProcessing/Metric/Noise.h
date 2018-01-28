#ifndef IMAGEPROCESSING_METRIC_NOISE_H
#define IMAGEPROCESSING_METRIC_NOISE_H

#include "ImageProcessing/Metric/MetricType.h"

namespace phobos { namespace iprocess { namespace metric {

namespace trait {
struct Noise
{
  static constexpr double minimum = 0.0;
  static constexpr double maximum = 7.0;

  using Comparing = comparing::SmallerIsBetter;
  using Aggregation = aggregation::Relative;
};
} // namespace trait

using Noise = MetricType<trait::Noise>;

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_NOISE_H
