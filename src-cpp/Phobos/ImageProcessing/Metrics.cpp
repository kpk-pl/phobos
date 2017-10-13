#include "Metrics.h"

namespace phobos { namespace iprocess {

boost::optional<double> Metric::score() const
{
  if (!seriesMetric)
    return boost::none;

  double result = 1.0;
  result *= seriesMetric->blur.value_or(1);
  result *= seriesMetric->noise.value_or(1);
  result *= seriesMetric->contrast.value_or(1);
  result *= seriesMetric->sharpness.value_or(1);
  result *= seriesMetric->depthOfField.value_or(1);
  return result;
}

}} // namespace phobos::iprocess
