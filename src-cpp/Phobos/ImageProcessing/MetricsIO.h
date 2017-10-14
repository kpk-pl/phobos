#ifndef IMAGE_PROCESSING_METRICS_IO_H
#define IMAGE_PROCESSING_METRICS_IO_H

#include <ostream>
#include "ImageProcessing/Metrics.h"

namespace phobos { namespace iprocess { namespace metric {

std::ostream& operator<<(std::ostream& os, MetricValues const& blur);
std::ostream& operator<<(std::ostream& os, Metric const& metric);
std::ostream& operator<<(std::ostream& os, MetricPtr metric);
std::ostream& operator<<(std::ostream& os, DepthOfField const& dof);

}}} // phobos::iprocess::metric

#endif // IMAGE_PROCESSING_METRICS_IO_H
