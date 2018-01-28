#ifndef IMAGEPROCESSING_METRIC_SATURATION_H
#define IMAGEPROCESSING_METRIC_SATURATION_H

#include "ImageProcessing/Metric/MetricType.h"

namespace phobos { namespace iprocess { namespace metric {

namespace trait {
struct Saturation
{
  static constexpr double minimum = 0.0;
  static constexpr double maximum = 255.0;

  using Comparing = comparing::BiggerIsBetter;
  using Aggregation = aggregation::Relative;
};
} // namespace trait

struct Saturation : public MetricType<trait::Saturation>
{
  using MetricType<trait::Saturation>::MetricType;

  bool isGrayscale() const
  {
    return value && *value < 0.001;
  }
};

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_SATURATION_H
