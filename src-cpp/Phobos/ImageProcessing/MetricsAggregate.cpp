#include "ImageProcessing/MetricsAggregate.h"

namespace phobos { namespace iprocess {

namespace {
    template <typename Func>
    auto maxMetric(std::vector<Metric const*> const& metrics, Func const& func)
        -> decltype(std::declval<Func>()(*metrics.front()))
    {
        auto maxIt = std::max_element(metrics.begin(), metrics.end(),
            [&func](Metric const* m1, Metric const* m2){ return func(*m1) < func(*m2); });
        return func(**maxIt);
    };

    template<typename Func>
    void aggregateMetric(std::vector<Metric const*> const& metrics, std::vector<ScoredMetric> &scored,
                         Func const& getter)
    {
        auto const maxM = maxMetric(metrics, getter);
        if (maxM == boost::none)
            return;

        assert(metrics.size() == scored.size());
        for (std::size_t i = 0; i < metrics.size(); ++i)
        {
            if (getter(*metrics[i]) != boost::none)
                getter(scored[i].seriesMetric) = *getter(*metrics[i]) / *maxM;
        }
    }
} // unnamed namespace

std::vector<ScoredMetric> aggregateMetrics(std::vector<Metric const*> const& metrics)
{
    std::vector<ScoredMetric> result;
    if (metrics.empty())
        return result;

    result.resize(metrics.size());

    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.blur.sobel; });
    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.blur.laplace; });
    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.blur.laplaceMod; });
    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.noise; });
    aggregateMetric(metrics, result, [](auto& m)->auto&{ return m.contrast; });

    auto& bestEl = *std::max_element(result.begin(), result.end(),
            [](ScoredMetric const& l, ScoredMetric const& r){ return l.score() < r.score(); });
    bestEl.bestQuality = true;

    return result;
}

}} // namespace phobos::iprocess

