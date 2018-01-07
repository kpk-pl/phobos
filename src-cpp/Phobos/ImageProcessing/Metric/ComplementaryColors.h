#ifndef IMAGEPROCESSING_METRIC_COMPLEMENTARY_COLORS_H
#define IMAGEPROCESSING_METRIC_COMPLEMENTARY_COLORS_H

#include "ImageProcessing/Metric/MetricType.h"

namespace phobos { namespace iprocess { namespace metric {

struct ComplementaryColors : public MetricType<ComplementaryColors>
{
  using MetricType::MetricType;

  static constexpr double minimum = 0.0;
  static constexpr double maximum = 1.0;

  using Comparing = comparing::BiggerIsBetter;
  using Aggregation = aggregation::Relative;
};

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_COMPLEMENTARY_COLORS_H
