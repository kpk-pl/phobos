#ifndef IMAGE_PROCESSING_METRICS_H
#define IMAGE_PROCESSING_METRICS_H

#include <memory>
#include <boost/optional.hpp>

namespace phobos { namespace iprocess {

struct Blur {
    boost::optional<double> sobel;
    boost::optional<double> laplace;
    boost::optional<double> laplaceMod;
};

struct SeriesMetric
{
    Blur blur;
    boost::optional<double> noise;
    boost::optional<double> contrast;
    double score() const;
};

struct Metric
{
    Blur blur;
    boost::optional<std::vector<float>> histogram;
    boost::optional<double> contrast;
    boost::optional<double> noise;
};

struct ScoredMetric
{
    SeriesMetric seriesMetric;
    bool bestQuality = false;
    inline double score() const { return seriesMetric.score(); }
};

using MetricPtr = std::shared_ptr<Metric>;
using MetricPtrVec = std::vector<MetricPtr>;
using ScoredMetricPtr = std::shared_ptr<ScoredMetric>;
using ScoredMetricPtrVec = std::vector<ScoredMetricPtr>;

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_METRICS_H
