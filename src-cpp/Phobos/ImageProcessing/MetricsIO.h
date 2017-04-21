#ifndef IMAGE_PROCESSING_METRICS_IO_H
#define IMAGE_PROCESSING_METRICS_IO_H

#include <ostream>
#include "ImageProcessing/Metrics.h"

namespace phobos { namespace iprocess {

std::ostream& operator<<(std::ostream& os, Blur const& blur);
std::ostream& operator<<(std::ostream& os, Metric const& metric);
std::ostream& operator<<(std::ostream& os, ScoredMetric const& scored);
std::ostream& operator<<(std::ostream& os, MetricPtr metric);
std::ostream& operator<<(std::ostream& os, ScoredMetricPtr scored);

}} // phobos::iprocess

#endif // IMAGE_PROCESSING_METRICS_IO_H