#ifndef IMAGEPROCESSING_FEATURE_HISTOGRAM_H
#define IMAGEPROCESSING_FEATURE_HISTOGRAM_H

#include <vector>
#include <map>

namespace phobos { namespace iprocess { namespace feature {

struct Histogram
{
  using DataType = std::vector<float>;
  enum Channel { Value, Red, Green, Blue };

  std::map<Channel, DataType> data;
  operator bool() const { return !data.empty(); }
};

}}} // namespace phobos::iprocess::feature

#endif // IMAGEPROCESSING_FEATURE_HISTOGRAM_H
