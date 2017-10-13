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

std::ostream& operator<<(std::ostream& os, DepthOfField const& dof)
{
  os << "{ "
     << "low: " << dof.low << ' '
     << "median: " << dof.median << ' '
     << "high: " << dof.high
     << " }";
  return os;
}

std::ostream& operator<<(std::ostream& os, MetricValues const& metric)
{
  os << "{ "
     << "blur: " << metric.blur << ' '
     << "noise: " << metric.noise << ' '
     << "contrast: " << metric.contrast << ' '
     << "sharpness: " << metric.sharpness << ' '
     << "depthOfField: " << metric.depthOfField
     << " }";
  return os;
}

std::ostream& operator<<(std::ostream& os, Metric const& scored)
{
  os << "{ "
     << "itemMetric: " << dynamic_cast<MetricValues const&>(scored) << ' '
     << "seriesMetric: " << scored.seriesMetric << ' '
     << "depthOfFieldRaw: " << scored.depthOfFieldRaw << ' '
     << "best: " << (scored.bestQuality ? "true" : "false") << ' '
     << "score: " << scored.score()
     << " }";

  return os;
}

std::ostream& operator<<(std::ostream& os, MetricPtr scored)
{
  if (scored) os << *scored;
  else        os << "none";
  return os;
}

}} // phobos::iprocess

