#ifndef IMAGE_PROCESSING_METRICS_H
#define IMAGE_PROCESSING_METRICS_H

#include "ImageProcessing/MetricsFwd.h"
#include <QMetaType>
#include <boost/optional.hpp>
#include <memory>
#include <tuple>

namespace phobos { namespace iprocess { namespace metric {

struct Histogram
{
  using DataType = std::vector<float>;
  enum Channel { Value, Red, Green, Blue };

  std::map<Channel, DataType> data;
  operator bool() const { return !data.empty(); }
};

struct DepthOfField
{
  DepthOfField() = default;
  DepthOfField(std::tuple<double, double, double> const& init)
  {
    std::tie(low, median, high) = init;
  }

  double low, median, high;
};

struct Hue
{
  static constexpr std::size_t const numberOfChannels = 6;
  enum class Name : std::size_t
  {
    Red = 0, Yellow, Green, Cyan, Blue, Magenta
  };

  std::array<double, numberOfChannels> channel;
};

struct MetricValues
{
  boost::optional<double>  blur;           // bigger is better
  boost::optional<double>  noise;          // smaller is better (but smaller means image is blurred)
  boost::optional<double>  contrast;       // bigger is better
  boost::optional<double>  sharpness;      // bigger is better
  boost::optional<double>  depthOfField;   // smaller is better
  boost::optional<double>  saturation;     // bigger is better
  boost::optional<double>  complementary;  // bigger is better
};

struct Metric : public MetricValues
{
  boost::optional<MetricValues> seriesMetric;

  Histogram histogram;
  boost::optional<DepthOfField> depthOfFieldRaw;
  boost::optional<Hue> hue;

  bool bestQuality = false;
  boost::optional<double> score() const;
};

}}} // namespace phobos::iprocess::metric

// TODO: Detect faces -> number of faces should be indicator if quality
// TODO: cumulative histogram calculate linearity -> indicator of quality
// TODO: Figure out some sort of Hough transform quality that tells how well-oriented the image is. Display angle the image is skewed.

Q_DECLARE_METATYPE(phobos::iprocess::metric::MetricPtr)

#endif // IMAGE_PROCESSING_METRICS_H
