#ifndef IMAGE_PROCESSING_METRICSAGGREGATE_H
#define IMAGE_PROCESSING_METRICSAGGREGATE_H

#include "ImageProcessing/Metrics.h"

namespace phobos { namespace iprocess {

std::vector<ScoredMetric> aggregateMetrics(std::vector<Metric const*> const& metrics);

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_METRICSAGGREGATE_H
