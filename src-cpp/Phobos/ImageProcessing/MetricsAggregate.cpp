#include "ImageProcessing/MetricsAggregate.h"

namespace phobos { namespace iprocess {

namespace {
    template <typename Func>
    auto maxMetric(MetricPtrVec const& metrics, Func const& func)
        -> decltype(std::declval<Func>()(*metrics.front()))
    {
        auto maxIt = std::max_element(metrics.begin(), metrics.end(),
            [&func](MetricPtr const& m1, MetricPtr const& m2){ return func(*m1) < func(*m2); });
        return func(**maxIt);
    };

    template<typename Func>
    void aggregateMetric(MetricPtrVec const& metrics, Func const& getter)
    {
        auto const maxM = maxMetric(metrics, getter);
        if (maxM == boost::none)
            return;

        for (std::size_t i = 0; i < metrics.size(); ++i)
        {
            if (getter(*metrics[i]) != boost::none)
                getter(metrics[i]->seriesMetric) = *getter(*metrics[i]) / *maxM;
        }
    }
} // unnamed namespace

void aggregateMetrics(MetricPtrVec const& metrics)
{
  if (metrics.empty())
    return;

  aggregateMetric(metrics, [](auto& m)->auto&{ return m.blur; });
  aggregateMetric(metrics, [](auto& m)->auto&{ return m.noise; });
  aggregateMetric(metrics, [](auto& m)->auto&{ return m.contrast; });

  auto& bestEl = *std::max_element(metrics.begin(), metrics.end(),
      [](MetricPtr const& l, MetricPtr const& r){ return l->score() < r->score(); });

  for (auto & metric : metrics)
    metric->bestQuality = (metric == bestEl);
}

}} // namespace phobos::iprocess

