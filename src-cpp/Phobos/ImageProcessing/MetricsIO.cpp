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

std::ostream& operator<<(std::ostream& os, MetricValues const& metric)
{
    os << "{" << std::endl
       << "blur: " << metric.blur << std::endl
       << "noise: " << metric.noise << std::endl
       << "contrast: " << metric.contrast << std::endl
       << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, Metric const& scored)
{
    os << "{" << std::endl
       << "itemMetric: " << dynamic_cast<MetricValues const&>(scored) << std::endl
       << "seriesMetric: " << scored.seriesMetric << std::endl
       << "best: " << (scored.bestQuality ? "true" : "false") << std::endl
       << "score: " << scored.score() << std::endl
       << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, MetricPtr scored)
{
    if (scored) os << *scored;
    else        os << "none";
    return os;
}

}} // phobos::iprocess

