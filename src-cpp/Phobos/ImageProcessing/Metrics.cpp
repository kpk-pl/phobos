#include "Metrics.h"

namespace phobos { namespace iprocess {

double SeriesMetric::score() const
{
    double result = 1.0;
    if (blur.sobel) result *= *blur.sobel;
    if (blur.laplace) result *= *blur.laplace;
    if (blur.laplaceMod) result *= *blur.laplaceMod;
    if (noise) result *= *noise;
    if (contrast) result *= *contrast;
    return result;
}

}} // namespace phobos::iprocess
