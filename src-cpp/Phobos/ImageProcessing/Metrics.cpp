#include "Metrics.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"

namespace phobos { namespace iprocess { namespace metric {

namespace {
struct Acc
{
  operator double() const
  {
    if (totalWeight == 0)
      return 0;
    return result / totalWeight;
  }

  void operator()(boost::optional<double> const& val, double const weight)
  {
    result += val.value_or(0) * weight;
    totalWeight += weight;
  }

private:
  double result = 0;
  double totalWeight = 0;
};
} // unnamed namespace

boost::optional<double> Metric::score() const
{
  if (!seriesMetric)
    return boost::none;

  config::ConfigPath const weightPath("metricWeights");
  Acc result;

  result(seriesMetric->blur, config::qualified(weightPath("blur"), 1.0));
  result(seriesMetric->noise, config::qualified(weightPath("noise"), 1.0));
  result(seriesMetric->contrast, config::qualified(weightPath("contrast"), 1.0));
  result(seriesMetric->sharpness, config::qualified(weightPath("sharpness"), 1.0));
  result(seriesMetric->depthOfField, config::qualified(weightPath("depthOfField"), 1.0));
  result(seriesMetric->saturation, config::qualified(weightPath("saturation"), 1.0));
  result(seriesMetric->complementary, config::qualified(weightPath("complementary"), 1.0));

  return double(result);
}

}}} // namespace phobos::iprocess::metric
