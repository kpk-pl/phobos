#include "ImageProcessing/Sharpness.h"
#include "ImageProcessing/Histogram.h"
#include <opencv2/opencv.hpp>
#include <cassert>
#include <set>
#include <array>
#include <algorithm>

namespace phobos { namespace iprocess { namespace sharpness {

namespace {
double nthPercentile(cv::Mat const& cvImage, float const n)
{
  assert(cvImage.depth() == CV_8U);
  assert(cvImage.channels() == 1);

  long const area = cvImage.rows * cvImage.cols;
  long const nthPercArea = n*area;

  cv::Mat const hist = histogram(cvImage, 256);

  long pixels = 0;
  long bin = 0;
  do
  {
    long const histVal = static_cast<long>(hist.at<float>(bin));
    if (pixels+histVal >= nthPercArea)
      break;

    pixels += histVal;
    ++bin;
  } while (bin < 256);

  return static_cast<double>(bin) + static_cast<double>(nthPercArea-pixels)/hist.at<float>(bin);
}

Result calculateAll(cv::Mat const& highImage)
{
  double const sharpPercentile = 0.98;

  Result result;
  result.sharpness = nthPercentile(highImage, sharpPercentile);

  std::size_t constexpr sideBreak = 5;
  std::size_t constexpr bins = sideBreak*sideBreak;
  auto const widthWide = highImage.cols / sideBreak;
  auto const heightWide = highImage.rows / sideBreak;

  std::array<double, bins> segmentSharpness;

  for (unsigned colPiece = 0; colPiece < sideBreak; ++colPiece)
    for (unsigned rowPiece = 0; rowPiece < sideBreak; ++rowPiece)
    {
      cv::Rect const roi(colPiece*widthWide, rowPiece*heightWide, widthWide, heightWide);
      segmentSharpness[colPiece*sideBreak+rowPiece] = nthPercentile(highImage(roi), sharpPercentile);
    }

  std::sort(std::begin(segmentSharpness), std::end(segmentSharpness));

  std::get<0>(result.breakout) = segmentSharpness.front();
  std::get<1>(result.breakout) = 0.5 * (segmentSharpness.at(bins / 2) + segmentSharpness.at((bins+1) / 2));
  std::get<2>(result.breakout) = segmentSharpness.back();

  return result;
}
} // unnamed namespace

Result homogeneous(cv::Mat const& cvImage, int const kernel)
{
  cv::Mat temp;
  cv::blur(cvImage, temp, cv::Size(kernel, kernel), cv::Point(-1, -1));
  temp = cvImage - temp;
  return calculateAll(temp);
}

Result gaussian(cv::Mat const& cvImage, int const kernel)
{
  cv::Mat temp;
  cv::GaussianBlur(cvImage, temp, cv::Size(kernel, kernel), 0, 0);
  temp = cvImage - temp;
  return calculateAll(temp);
}

}}} // namespace phobos::iprocess::sharpness
