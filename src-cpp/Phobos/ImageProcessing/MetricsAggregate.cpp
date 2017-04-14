#include "ImageProcessing/MetricsAggregate.h"

namespace phobos { namespace iprocess {

namespace {
    Metric calcMaxMetric(MetricPtrVec const& metrics)
    {
        Metric result;
       /* maxMet.blurSobel = max([item.blurSobel for item in metrics])
        maxMet.blurLaplace = max([item.blurLaplace for item in metrics])
        maxMet.blurLaplaceMod = max([item.blurLaplaceMod for item in metrics])
        maxMet.noise = max([item.noise for item in metrics])
        maxMet.contrast = max([item.contrast for item in metrics]) */
        return result;
    }
} // unnamed namespace

ScoredMetricPtrVec aggregateMetrics(MetricPtrVec const& metrics)
{
    ScoredMetricPtrVec result;
    result.reserve(metrics.size());

    Metric const maxMetric = calcMaxMetric(metrics);

    for (auto const& metric : metrics)
    {
        ScoredMetricPtr scored = std::make_shared<ScoredMetric>();
        /*
        newItem.seriesAggregated = SeriesMetrics()
        newItem.seriesAggregated.blurSobelPrc = item.blurSobel / maxMet.blurSobel
        newItem.seriesAggregated.blurLaplacePrc = item.blurLaplace / maxMet.blurLaplace
        newItem.seriesAggregated.blurLaplaceModPrc = item.blurLaplaceMod / maxMet.blurLaplaceMod
        newItem.seriesAggregated.noisePrc = item.noise / maxMet.noise
        newItem.seriesAggregated.contrastPrc = item.contrast / maxMet.contrast */

        result.push_back(scored);
    }

    /*
     *     bestItem = max(result, key=lambda i: i.quality())
    bestItem.bestQuality = True */

    return result;
}

}} // namespace phobos::iprocess

