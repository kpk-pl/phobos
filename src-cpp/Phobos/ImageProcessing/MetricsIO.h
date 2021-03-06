#ifndef IMAGE_PROCESSING_METRICS_IO_H
#define IMAGE_PROCESSING_METRICS_IO_H

#include "ImageProcessing/Metrics.h"
#include <ostream>

namespace phobos { namespace iprocess {

std::ostream& operator<<(std::ostream& os, MetricSeriesScores const& blur);
std::ostream& operator<<(std::ostream& os, Metric const& metric);
std::ostream& operator<<(std::ostream& os, MetricPtr metric);

}} // phobos::iprocess

#endif // IMAGE_PROCESSING_METRICS_IO_H
