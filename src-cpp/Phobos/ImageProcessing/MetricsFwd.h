#ifndef IMAGEPROCESSING_METRICSFWD_H_
#define IMAGEPROCESSING_METRICSFWD_H_

#include <memory>
#include <vector>

namespace phobos { namespace iprocess { namespace metric {

struct Metric;
typedef std::shared_ptr<Metric> MetricPtr;
typedef std::vector<MetricPtr> MetricPtrVec;

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRICSFWD_H_
