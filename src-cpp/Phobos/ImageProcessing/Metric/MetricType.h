#ifndef IMAGEPROCESSING_METRIC_TYPE_H
#define IMAGEPROCESSING_METRIC_TYPE_H

#include "ImageProcessing/Metric/Traits.h"
#include <boost/optional.hpp>
#include <type_traits>

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

template<typename Trait>
struct MetricType : public Trait
{
  static_assert(std::is_arithmetic<decltype(Trait::minimum)>::value, "Metric trait must define arithmetic minimum");
  static_assert(std::is_arithmetic<decltype(Trait::maximum)>::value, "Metric trait must define arithmetic maximum");
  static_assert(std::is_class<typename Trait::Comparing>::value, "Metric trait must define a Comparing type");
  static_assert(std::is_class<typename Trait::Aggregation>::value, "Metric trait must define an Aggregation type");

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

    return _detail::score(*value, Trait::minimum, Trait::maximum, typename Trait::Comparing{});
  }

  boost::optional<double> value;
};

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_TYPE_H
