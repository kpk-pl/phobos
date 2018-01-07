#include "ImageProcessing/Calculator/Hue.h"
#include <opencv2/opencv.hpp>

namespace phobos { namespace iprocess { namespace calc {

namespace {
cv::Mat hueHistogram(cv::Mat const& hsvImage, int const bins)
{
  int channels[] = { 0 };
  float hranges[] = { 0, 180 };
  const float* ranges[] = { hranges };

  cv::Mat hHist;
  calcHist(&hsvImage, 1, channels, cv::Mat(), hHist, 1, &bins, ranges);

  return hHist;
}
} // unnamed namespace

feature::Hue hueChannels(cv::Mat const& hsvImage)
{
  static_assert(feature::Hue::numberOfChannels == 6, "Wrong number of channels in Hue calculation");

  cv::Mat const hueHist = hueHistogram(hsvImage, 6);
  float const imageArea = hsvImage.rows * hsvImage.cols;

  feature::Hue hue;
  for (std::size_t ch = 0; ch < 6; ++ch)
    hue.channel[ch] = hueHist.at<float>(ch) / imageArea;

  return hue;
}

metric::ComplementaryColors complementaryChannels(feature::Hue const& hue)
{
  double result = 0;

  auto const half = hue.numberOfChannels / 2;
  for (std::size_t i = 0; i < half; ++i)
    result += hue.channel[i] * hue.channel[i + half];

  /*
   * The highest possible number from the above calculation is 1/4
   */
  return metric::ComplementaryColors{result * 4.0};
}

}}} // namespace phobos::iprocess::calc
