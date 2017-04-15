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
    void aggregateMetric(MetricPtrVec const& metrics, ScoredMetricPtrVec &scored,
                         Func const& getter)
    {
        auto const maxM = maxMetric(metrics, getter);
        if (maxM == boost::none)
            return;

        assert(metrics.size() == scored.size());
        for (std::size_t i = 0; i < metrics.size(); ++i)
        {
            if (getter(*metrics[i]) != boost::none)
                getter(scored[i]->seriesMetric) = *getter(*metrics[i]) / *maxM;
        }
    }
} // unnamed namespace

ScoredMetricPtrVec aggregateMetrics(MetricPtrVec const& metrics)
{
    ScoredMetricPtrVec result;
    result.reserve(metrics.size());
    if (metrics.empty())
        return result;

    for (std::size_t i = 0; i < metrics.size(); ++i)
        result.emplace_back(std::make_shared<ScoredMetric>());

    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.blur.sobel; });
    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.blur.laplace; });
    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.blur.laplaceMod; });
    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.noise; });
    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.contrast; });

    auto const& bestEl = *std::max_element(result.begin(), result.end(),
            [](ScoredMetricPtr const& l, ScoredMetricPtr const& r){ return l->score() < r->score(); });
    bestEl->bestQuality = true;

    return result;
}

}} // namespace phobos::iprocess

