#include "ImageProcessing/MetricsIO.h"

namespace phobos { namespace iprocess { namespace metric {

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

std::ostream& operator<<(std::ostream& os, Hue const& hue)
{
  static_assert(Hue::numberOfChannels == 6, "Support for only 6 channels");

  os << "{ "
     << "red: " << hue.getChannel(Hue::Name::Red) << ' '
     << "yellow: " << hue.getChannel(Hue::Name::Yellow) << ' '
     << "green: " << hue.getChannel(Hue::Name::Green) << ' '
     << "cyan: " << hue.getChannel(Hue::Name::Cyan) << ' '
     << "blue: " << hue.getChannel(Hue::Name::Blue) << ' '
     << "magenta: " << hue.getChannel(Hue::Name::Magenta)
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
     << "depthOfField: " << metric.depthOfField << ' '
     << "saturation: " << metric.saturation << ' '
     << "complementary: " << metric.complementary
     << " }";
  return os;
}

std::ostream& operator<<(std::ostream& os, Metric const& scored)
{
  os << "{ "
     << "itemMetric: " << dynamic_cast<MetricValues const&>(scored) << ' '
     << "seriesMetric: " << scored.seriesMetric << ' '
     << "depthOfFieldRaw: " << scored.depthOfFieldRaw << ' '
     << "hue: " << scored.hue << ' '
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

}}} // phobos::iprocess::metric
