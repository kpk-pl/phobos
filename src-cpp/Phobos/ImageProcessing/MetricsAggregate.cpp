#include "ImageProcessing/MetricsAggregate.h"
#include <algorithm>
#include <type_traits>

namespace phobos { namespace iprocess {

namespace {

// TODO: Allow indicating that loaded photos are not a series
// In this case don't perform series aggregation (no best mark feature too)
// But display the picture score only
// Maybe such series can be distinguished on a label in first column in allseriesview

namespace detail {
template<typename M>
auto source(MetricPtr const& metric, M Metric::*ptr)
  -> typename std::add_lvalue_reference<decltype((*metric).*ptr)>::type
{
  return (*metric).*ptr;
}

template<typename S>
auto target(MetricPtr const& metric, S MetricSeriesScores::*ptr)
  -> typename std::add_lvalue_reference<decltype((metric->seriesScores.get()).*ptr)>::type
{
  return (metric->seriesScores.get()).*ptr;
}
} // namespace detail

template<typename T>
void aggregateMetric(T& target, boost::optional<double> const& minValue, boost::optional<double> const& maxValue)
{
  if (!minValue || !maxValue || !target)
  {
    target = boost::none;
    return;
  }

  if (*maxValue == *minValue)
  {
    target = 1.0;
    return;
  }

  target = (*target - *minValue) / (*maxValue - *minValue);
}

template<typename M, typename S>
void processMetric(MetricPtrVec const& metrics, M Metric::*source, S MetricSeriesScores::*target, metric::aggregation::Absolute)
{
  for (MetricPtr const& metric : metrics)
    detail::target(metric, target) = detail::source(metric, source).score();
}

template<typename M, typename S>
void processMetric(MetricPtrVec const& metrics, M Metric::*source, S MetricSeriesScores::*target, metric::aggregation::Relative)
{
  for (MetricPtr const& metric : metrics)
    detail::target(metric, target) = detail::source(metric, source).score();

  auto minmaxIt = std::minmax_element(metrics.begin(), metrics.end(),
      [&](MetricPtr const& m1, MetricPtr const& m2){ return detail::target(m1, target) < detail::target(m2, target); });

  auto const minValue = detail::target(*minmaxIt.first, target);
  auto const maxValue = detail::target(*minmaxIt.second, target);

  for (MetricPtr const& metric : metrics)
    aggregateMetric(detail::target(metric, target), minValue, maxValue);
}

template<typename M, typename S>
void processMetric(MetricPtrVec const& metrics, M Metric::*source, S MetricSeriesScores::*target)
{
  processMetric(metrics, source, target, typename M::Aggregation());
}

} // unnamed namespace

void aggregate(MetricPtrVec const& metrics)
{
  if (metrics.empty())
    return;

  for (auto &metric : metrics)
    metric->seriesScores = MetricSeriesScores{};

  processMetric(metrics, &Metric::blur, &MetricSeriesScores::blur);
  processMetric(metrics, &Metric::noise, &MetricSeriesScores::noise);
  processMetric(metrics, &Metric::contrast, &MetricSeriesScores::contrast);
  processMetric(metrics, &Metric::sharpness, &MetricSeriesScores::sharpness);
  processMetric(metrics, &Metric::depthOfField, &MetricSeriesScores::depthOfField);
  processMetric(metrics, &Metric::saturation, &MetricSeriesScores::saturation);
  processMetric(metrics, &Metric::complementary, &MetricSeriesScores::complementary);

  auto& bestEl = *std::max_element(metrics.begin(), metrics.end(),
      [](MetricPtr const& l, MetricPtr const& r){ return l->seriesScores->score() < r->seriesScores->score(); });
  bestEl->seriesScores->bestQuality = true;
}

}} // namespace phobos::iprocess
