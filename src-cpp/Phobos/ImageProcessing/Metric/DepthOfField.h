#ifndef IMAGEPROCESSING_METRIC_DEPTH_OF_FIELD_H
#define IMAGEPROCESSING_METRIC_DEPTH_OF_FIELD_H

#include "ImageProcessing/Metric/MetricType.h"
#include <boost/optional.hpp>
#include <cmath>

namespace phobos { namespace iprocess { namespace metric {

struct DepthOfField : public MetricType<DepthOfField>
{
  static constexpr double minimum = 0.0;
  static constexpr double maximum = 7.0;

  using Comparing = comparing::SmallerIsBetter;
  using Aggregation = aggregation::Relative;

  DepthOfField() = default;
  DepthOfField(double const low, double const median, double const high)
    : MetricType(std::log2(high/median)), low(low), median(median), high(high)
  {}

  boost::optional<double> low;
  boost::optional<double> median;
  boost::optional<double> high;
};

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_DEPTH_OF_FIELD_H
