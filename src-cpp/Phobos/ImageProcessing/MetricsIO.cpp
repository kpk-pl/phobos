#include "ImageProcessing/MetricsIO.h"
#include "ImageProcessing/Feature/Hue.h"

namespace phobos { namespace iprocess {

namespace {
template <typename T>
std::ostream& operator<<(std::ostream& os, boost::optional<T> const& el)
{
  if (el) os << *el;
  else    os << "none";
  return os;
}
template<typename T>
std::ostream& operator<<(std::ostream& os, metric::MetricType<T> const& m)
{
  os << m.value;
  return os;
}
std::ostream& operator<<(std::ostream& os, metric::DepthOfField const& dof)
{
  os << "{ "
     << "value: " << dof.value << ' '
     << "low: " << dof.low << ' '
     << "median: " << dof.median << ' '
     << "high: " << dof.high
     << " }";
  return os;
}
std::ostream& operator<<(std::ostream& os, metric::Saturation const& sat)
{
  os << "{ "
     << "value: " << sat.value << ' '
     << "isGrayscale: " << sat.isGrayscale()
     << " }";
  return os;
}

std::ostream& operator<<(std::ostream& os, feature::Hue const& hue)
{
  using feature::Hue;
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
} // unnamed namespace

std::ostream& operator<<(std::ostream& os, MetricSeriesScores const& metric)
{
  os << "{ "
     << "best: " << metric.bestQuality << ' '
     << "score: " << metric.score() << ' '
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
     << "blur: " << scored.blur << ' '
     << "noise: " << scored.noise << ' '
     << "contrast: " << scored.contrast << ' '
     << "sharpness: " << scored.sharpness << ' '
     << "depthOfField: " << scored.depthOfField << ' '
     << "saturation: " << scored.saturation << ' '
     << "complementary: " << scored.complementary << ' '
     << "seriesScores: " << scored.seriesScores << ' '
     << "hue: " << scored.hue
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
