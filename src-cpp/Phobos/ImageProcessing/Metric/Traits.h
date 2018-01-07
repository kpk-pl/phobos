#ifndef IMAGEPROCESSING_METRIC_TRAITS_H
#define IMAGEPROCESSING_METRIC_TRAITS_H

namespace phobos { namespace iprocess { namespace metric {

namespace comparing {
struct BiggerIsBetter{};
struct SmallerIsBetter{};
} // namespace comparing

namespace aggregation {
struct Absolute{};
struct Relative{};
} // namespace aggregation

}}} // namespace phobos::iprocess::metric

#endif // IMAGEPROCESSING_METRIC_TRAITS_H
