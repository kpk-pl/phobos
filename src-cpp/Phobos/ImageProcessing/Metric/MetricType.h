#ifndef IMAGEPROCESSING_METRIC_TYPE_H
#define IMAGEPROCESSING_METRIC_TYPE_H

#include "ImageProcessing/Metric/Traits.h"
#include <boost/optional.hpp>

namespace phobos { namespace iprocess { namespace metric {

namespace _detail {
inline double score(double const value, double const min, double const max, comparing::BiggerIsBetter)
{
  if (value < min)
    return 0.0;
  if (value >= max)
    return 1.0;
  if (max == min)
    return 1.0;
  return (value - min) / (max - min);
}
inline double score(double const value, double const min, double const max, comparing::SmallerIsBetter)
{
  return score(-value, -max, -min, comparing::BiggerIsBetter{});
}
} // namespace _detail

// TODO: Each metric should be a base class
// Then there should be another class deriving from T - the metric
// It can then implement all the methods needed here (score and operators)
// as well as static_assert on features inside the metric class

template<typename Derived>
struct MetricType
{
  MetricType() = default;
  MetricType(double const v) : value(v)
  {}

  bool operator<(MetricType const& other) const
  {
    return value < other.value;
  }

  bool operator==(MetricType const& other) const
  {
    return value == other.value;
  }

  boost::optional<double> score() const
  {
    if (!value)
      return boost::none;

    return _detail::score(*value, Derived::minimum, Derived::maximum, typename Derived::Comparing{});
  }

  boost::optional<double> value;
};

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_TYPE_H
