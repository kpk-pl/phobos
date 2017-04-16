#include "ImageProcessing/MetricsIO.h"

namespace phobos { namespace iprocess {

namespace {
template <typename T>
std::ostream& operator<<(std::ostream& os, boost::optional<T> const& el)
{
    if (el) os << *el;
    else    os << "none";
    return os;
}
} // unnamed namespace

std::ostream& operator<<(std::ostream& os, Blur const& blur)
{
    os << "{ sobel: " << blur.sobel
       << " laplace: " << blur.laplace
       << " laplaceMod: " << blur.laplaceMod << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, Metric const& metric)
{
    os << "{" << std::endl
       << "blur: " << metric.blur << std::endl
       << "noise: " << metric.noise << std::endl
       << "contrast: " << metric.contrast << std::endl
       << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, MetricPtr metric)
{
    if (metric) os << *metric;
    else        os << "none";
    return os;
}

std::ostream& operator<<(std::ostream& os, ScoredMetric const& scored)
{
    os << "{" << std::endl
       << "seriesMetric: " << scored.seriesMetric << std::endl
       << "best: " << (scored.bestQuality ? "true" : "false") << std::endl
       << "score: " << scored.score() << std::endl
       << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, ScoredMetricPtr scored)
{
    if (scored) os << *scored;
    else        os << "none";
    return os;
}

}} // phobos::iprocess

