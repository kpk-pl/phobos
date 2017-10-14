#include "ImageProcessing/MetricsAggregate.h"
#include <algorithm>
#include <type_traits>

namespace phobos { namespace iprocess { namespace metric {

namespace {

template<typename Func>
using FuncRes = std::decay_t<std::result_of_t<Func(Metric&)>>;

template<typename Func>
using FuncResPair = std::pair<FuncRes<Func>, FuncRes<Func>>;

template <typename Func>
auto minMaxMetric(MetricPtrVec const& metrics, Func const& func)
    -> FuncResPair<Func>
{
  auto minmaxIt = std::minmax_element(metrics.begin(), metrics.end(),
      [&func](MetricPtr const& m1, MetricPtr const& m2){ return func(*m1) < func(*m2); });
  return std::make_pair(func(**minmaxIt.first), func(**minmaxIt.second));
};

struct BiggerIsBetter{};
struct SmallerIsBetter{};

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
      return 0.0;

    return (*value - *minMax.first) / (*minMax.second - *minMax.first);
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
      return 0.0;

    return (*minMax.second - *value) / (*minMax.second - *minMax.first);
  }
};

template<typename Type, typename Func>
void aggregateMetric(MetricPtrVec const& metrics, Func const& getter)
{
  auto const minmaxM = minMaxMetric(metrics, getter);
  if (minmaxM.first == boost::none || minmaxM.second == boost::none)
    return;

  for (std::size_t i = 0; i < metrics.size(); ++i)
    getter(*metrics[i]->seriesMetric) = RelativeValue<Type>{}(getter(*metrics[i]), minmaxM);
}
} // unnamed namespace

// TODO: Need to examine if scoring by values linearily is the best approach
void aggregate(MetricPtrVec const& metrics)
{
  if (metrics.empty())
    return;

  for (auto &metric : metrics)
    metric->seriesMetric = MetricValues{};

  aggregateMetric<BiggerIsBetter>(metrics,  [](auto& m)->auto&{ return m.blur; });
  aggregateMetric<SmallerIsBetter>(metrics, [](auto& m)->auto&{ return m.noise; });
  aggregateMetric<BiggerIsBetter>(metrics,  [](auto& m)->auto&{ return m.contrast; });
  aggregateMetric<BiggerIsBetter>(metrics,  [](auto& m)->auto&{ return m.sharpness; });
  aggregateMetric<SmallerIsBetter>(metrics, [](auto& m)->auto&{ return m.depthOfField; });

  auto& bestEl = *std::max_element(metrics.begin(), metrics.end(),
      [](MetricPtr const& l, MetricPtr const& r){ return l->score() < r->score(); });

  for (auto & metric : metrics)
    metric->bestQuality = (metric == bestEl);
}

}}} // namespace phobos::iprocess::metric

