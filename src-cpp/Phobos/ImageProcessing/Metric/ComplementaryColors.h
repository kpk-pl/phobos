#ifndef IMAGEPROCESSING_METRIC_COMPLEMENTARY_COLORS_H
#define IMAGEPROCESSING_METRIC_COMPLEMENTARY_COLORS_H

#include "ImageProcessing/Metric/MetricType.h"

namespace phobos { namespace iprocess { namespace metric {

namespace trait {
struct ComplementaryColors
{
  static constexpr double minimum = 0.0;
  static constexpr double maximum = 1.0;

  using Comparing = comparing::BiggerIsBetter;
  using Aggregation = aggregation::Relative;
};
} // namespace trait

using ComplementaryColors = MetricType<trait::ComplementaryColors>;

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_COMPLEMENTARY_COLORS_H
