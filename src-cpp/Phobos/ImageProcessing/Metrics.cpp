#include "Metrics.h"

namespace phobos { namespace iprocess {

double ScoredMetric::score() const
{
    double result = 1.0;
    result *= seriesMetric.blur.sobel.value_or(1);
    result *= seriesMetric.blur.laplace.value_or(1);
    result *= seriesMetric.blur.laplaceMod.value_or(1);
    result *= seriesMetric.noise.value_or(1);
    result *= seriesMetric.contrast.value_or(1);
    return result;
}

}} // namespace phobos::iprocess
