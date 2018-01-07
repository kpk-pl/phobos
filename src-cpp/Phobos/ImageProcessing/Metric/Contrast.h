#ifndef IMAGEPROCESSING_METRIC_CONTRAST_H
#define IMAGEPROCESSING_METRIC_CONTRAST_H

#include "ImageProcessing/Metric/MetricType.h"

namespace phobos { namespace iprocess { namespace metric {

struct Contrast : public MetricType<Contrast>
{
  using MetricType::MetricType;

  static constexpr double minimum = 0.06;
  static constexpr double maximum = 0.39;

  using Comparing = comparing::BiggerIsBetter;
  using Aggregation = aggregation::Relative;
};

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_CONTRAST_H
