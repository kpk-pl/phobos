#include "ImageProcessing/MetricsAggregate.h"
#include <algorithm>
#include <type_traits>

namespace phobos { namespace iprocess { namespace metric {

namespace {

template <typename T>
std::pair<T,T> minMaxMetric(MetricPtrVec const& metrics, T MetricValues::*member)
{
  auto minmaxIt = std::minmax_element(metrics.begin(), metrics.end(),
      [&](MetricPtr const& m1, MetricPtr const& m2){ return (*m1).*member < (*m2).*member; });
  return std::make_pair((**minmaxIt.first).*member, (**minmaxIt.second).*member);
};

struct BiggerIsBetter{};
struct SmallerIsBetter{};
struct ZeroToOneBiggerIsBetter{};

template<typename T>
struct RelativeValue;

template<>
struct RelativeValue<BiggerIsBetter>
{
  template<typename OptV>
  OptV operator()(OptV const& value, std::pair<OptV, OptV> const& minMax) const
  {
    if (value == boost::none)
      return boost::none;

    if (minMax.first == minMax.second)
      return 1.0;

    return (*value - *minMax.first) / (*minMax.second - *minMax.first);
  }
};

template<>
struct RelativeValue<ZeroToOneBiggerIsBetter>
{
  template<typename OptV>
  OptV operator()(OptV const& value, std::pair<OptV, OptV> const&) const
  {
    if (value == boost::none)
      return boost::none;

    if (*value > 1.0)
      return 1.0;
    else if (*value < 0.0)
      return 0.0;

    return *value;
  }
};

template<>
struct RelativeValue<SmallerIsBetter>
{
  template<typename OptV>
  OptV operator()(OptV const& value, std::pair<OptV, OptV> const& minMax) const
  {
    if (value == boost::none)
      return boost::none;

    if (minMax.first == minMax.second)
      return 1.0;

    return (*minMax.second - *value) / (*minMax.second - *minMax.first);
  }
};

template<typename Type, typename T>
void aggregateMetric(MetricPtrVec const& metrics, T MetricValues::*member)
{
  auto const minmaxM = minMaxMetric(metrics, member);
  if (minmaxM.first == boost::none || minmaxM.second == boost::none)
    return;

  for (std::size_t i = 0; i < metrics.size(); ++i)
    (*metrics[i]->seriesMetric).*member = RelativeValue<Type>{}((*metrics[i]).*member, minmaxM);
}
} // unnamed namespace

// TODO: Need to examine if scoring by values linearily is the best approach
void aggregate(MetricPtrVec const& metrics)
{
  if (metrics.empty())
    return;

  for (auto &metric : metrics)
    metric->seriesMetric = MetricValues{};

  aggregateMetric<BiggerIsBetter>(metrics, &Metric::blur);
  aggregateMetric<SmallerIsBetter>(metrics, &Metric::noise);
  aggregateMetric<BiggerIsBetter>(metrics, &Metric::contrast);
  aggregateMetric<BiggerIsBetter>(metrics, &Metric::sharpness);
  aggregateMetric<SmallerIsBetter>(metrics, &Metric::depthOfField);
  aggregateMetric<BiggerIsBetter>(metrics, &Metric::saturation);
  aggregateMetric<ZeroToOneBiggerIsBetter>(metrics, &Metric::complementary);

  auto& bestEl = *std::max_element(metrics.begin(), metrics.end(),
      [](MetricPtr const& l, MetricPtr const& r){ return l->score() < r->score(); });

  for (auto & metric : metrics)
    metric->bestQuality = (metric == bestEl);
}

}}} // namespace phobos::iprocess::metric

