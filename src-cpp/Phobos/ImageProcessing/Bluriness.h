#ifndef IMAGE_PROCESSING_BLURINESS_H
#define IMAGE_PROCESSING_BLURINESS_H

#include <opencv2/core/core.hpp>

/*
 * Adapted from:
 * http://stackoverflow.com/questions/7765810/is-there-a-way-to-detect-if-an-image-is-blurry/7768918#7768918
 */

namespace phobos { namespace iprocess { namespace blur {

struct Sobel{};
struct Laplace{};
struct LaplaceMod{};

} // namespace blur

template<typename Strategy>
struct Bluriness
{
  // depth of cv.CV_32F allocates a lot of memory
  Bluriness(std::size_t const N, int depth = -1) :
    _n(N), _depth(depth)
  {}

  // Returns smaller values for blurry images
  double operator()(cv::Mat const& cvImage) const;

protected:
  double calc(cv::Mat const& cvImage) const;

  std::size_t const _n;
  int const _depth;
};

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_BLURINESS_H
