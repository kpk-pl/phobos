#ifndef IMAGEPROCESSING_METRIC_BLUR_H
#define IMAGEPROCESSING_METRIC_BLUR_H

#include "ImageProcessing/Metric/MetricType.h"

namespace phobos { namespace iprocess { namespace metric {

struct Blur : public MetricType<Blur>
{
  using MetricType::MetricType;

  static constexpr double minimum = 0.0;
  static constexpr double maximum = 16384.0;

  using Comparing = comparing::BiggerIsBetter;
  using Aggregation = aggregation::Relative;
};

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_BLUR_H
