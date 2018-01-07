#include "ImageProcessing/Metrics.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"

namespace phobos { namespace iprocess {

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

double MetricSeriesScores::score() const
{
  config::ConfigPath const weightPath("metricWeights");
  Acc result;

  result(blur, config::qualified(weightPath("blur"), 1.0));
  result(noise, config::qualified(weightPath("noise"), 1.0));
  result(contrast, config::qualified(weightPath("contrast"), 1.0));
  result(sharpness, config::qualified(weightPath("sharpness"), 1.0));
  result(depthOfField, config::qualified(weightPath("depthOfField"), 1.0));
  result(saturation, config::qualified(weightPath("saturation"), 1.0));
  result(complementary, config::qualified(weightPath("complementary"), 1.0));

  return result;
}

}} // namespace phobos::iprocess
