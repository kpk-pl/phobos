#ifndef IMAGEPROCESSING_METRIC_CALCULATOR_H
#define IMAGEPROCESSING_METRIC_CALCULATOR_H

#include "ImageProcessing/MetricsFwd.h"
#include <opencv2/core/core.hpp>

namespace phobos { namespace iprocess {

MetricPtr calcMetrics(cv::Mat image);

}} // namespace phobos::iprocess

#endif // IMAGEPROCESSING_METRIC_CALCULATOR_H
