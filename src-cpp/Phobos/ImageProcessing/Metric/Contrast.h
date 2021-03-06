#ifndef IMAGEPROCESSING_METRIC_CONTRAST_H
#define IMAGEPROCESSING_METRIC_CONTRAST_H

#include "ImageProcessing/Metric/MetricType.h"

namespace phobos { namespace iprocess { namespace metric {

namespace trait {
struct Contrast
{
  // for 256 bins 0-1 normalized histogram
  static constexpr double minimum = 0.0625;
  static constexpr double maximum = 0.5;

  using Comparing = comparing::BiggerIsBetter;
  using Aggregation = aggregation::Relative;
};
} // namespace trait

using Contrast = MetricType<trait::Contrast>;

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_CONTRAST_H
