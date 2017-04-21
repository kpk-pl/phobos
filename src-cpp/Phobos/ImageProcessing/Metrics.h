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

struct Metric
{
    Blur blur;
    boost::optional<double> noise;
    boost::optional<double> contrast;
    boost::optional<std::vector<float>> histogram;
};

struct ScoredMetric
{
    Metric seriesMetric;
    bool bestQuality = false;
    double score() const;
};

typedef std::shared_ptr<Metric> MetricPtr ;
typedef std::vector<MetricPtr> MetricPtrVec;
typedef std::shared_ptr<ScoredMetric> ScoredMetricPtr;
typedef std::vector<ScoredMetricPtr> ScoredMetricPtrVec;

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_METRICS_H