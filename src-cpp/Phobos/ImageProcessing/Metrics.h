#ifndef IMAGE_PROCESSING_METRICS_H
#define IMAGE_PROCESSING_METRICS_H

#include "ImageProcessing/MetricsFwd.h"
#include "ImageProcessing/Metric/All.h"
#include "ImageProcessing/Feature/All.h"
#include <QMetaType>
#include <boost/optional.hpp>
#include <memory>
#include <tuple>

namespace phobos { namespace iprocess {

/*
 * Scores relative to the whole photoseries. Each value can be either not available
 * or have value between 0.0 (worst) - 1.0 (best)
 */
struct MetricSeriesScores
{
  boost::optional<double> blur;
  boost::optional<double> noise;
  boost::optional<double> contrast;
  boost::optional<double> sharpness;
  boost::optional<double> depthOfField;
  boost::optional<double> saturation;
  boost::optional<double> complementary;

  bool bestQuality = false;
  double score() const;
};

struct Metric
{
  metric::Blur                blur;
  metric::Noise               noise;
  metric::Contrast            contrast;
  metric::Sharpness           sharpness;
  metric::DepthOfField        depthOfField;
  metric::Saturation          saturation;
  metric::ComplementaryColors complementary;

  boost::optional<feature::Hue> hue;
  feature::Histogram histogram;

  boost::optional<MetricSeriesScores> seriesScores;
};

}} // namespace phobos::iprocess

// TODO: Detect faces -> number of faces should be indicator if quality
// TODO: cumulative histogram calculate linearity -> indicator of quality
// TODO: Figure out some sort of Hough transform quality that tells how well-oriented the image is. Display angle the image is skewed.

Q_DECLARE_METATYPE(phobos::iprocess::MetricPtr)

#endif // IMAGE_PROCESSING_METRICS_H
