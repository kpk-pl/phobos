#include "Metrics.h"

namespace phobos { namespace iprocess { namespace metric {

boost::optional<double> Metric::score() const
{
  if (!seriesMetric)
    return boost::none;
  // TODO: weighted score
  double result = 0;
  result += seriesMetric->blur.value_or(0);
  result += seriesMetric->noise.value_or(0);
  result += seriesMetric->contrast.value_or(0);
  result += seriesMetric->sharpness.value_or(0);
  result += seriesMetric->depthOfField.value_or(0);
  return result / 5.0;
}

}}} // namespace phobos::iprocess::metric
