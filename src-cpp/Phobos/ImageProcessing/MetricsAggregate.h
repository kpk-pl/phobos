#ifndef IMAGE_PROCESSING_METRICSAGGREGATE_H
#define IMAGE_PROCESSING_METRICSAGGREGATE_H

#include "ImageProcessing/Metrics.h"

namespace phobos { namespace iprocess {

ScoredMetricPtrVec aggregateMetrics(MetricPtrVec const& metrics);

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_METRICSAGGREGATE_H
