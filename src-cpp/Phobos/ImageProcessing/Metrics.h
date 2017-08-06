#ifndef IMAGE_PROCESSING_METRICS_H
#define IMAGE_PROCESSING_METRICS_H

#include <QMetaType>
#include <memory>
#include <boost/optional.hpp>
#include "ImageProcessing/MetricsFwd.h"

namespace phobos { namespace iprocess {

struct Blur {
    boost::optional<double> sobel;
    boost::optional<double> laplace;
    boost::optional<double> laplaceMod;
};

struct MetricValues
{
    Blur blur;
    boost::optional<double> noise;
    boost::optional<double> contrast;
};

struct Metric : public MetricValues
{
    MetricValues seriesMetric;
    boost::optional<std::vector<float>> histogram;
    bool bestQuality = false;
    double score() const;
};

}} // namespace phobos::iprocess

// TODO: Detect faces -> number of faces should be indicator if quality
// TODO: cumulative histogram calculate linearity -> indicator of quality
// TODO: Figure out some sort of Hough transform quality that tells how well-oriented the image is. Display angle the image is skewed.

Q_DECLARE_METATYPE(phobos::iprocess::MetricPtr)

#endif // IMAGE_PROCESSING_METRICS_H
