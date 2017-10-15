#include "ImageProcessing/Hue.h"
#include <opencv2/opencv.hpp>

namespace phobos { namespace iprocess {

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

metric::Hue hueChannels(cv::Mat const& hsvImage)
{
  static_assert(metric::Hue::numberOfChannels == 6, "Wrong number of channels in Hue calculation");

  cv::Mat const hueHist = hueHistogram(hsvImage, 6);
  float const imageArea = hsvImage.rows * hsvImage.cols;

  metric::Hue hue;
  for (std::size_t ch = 0; ch < 6; ++ch)
    hue.channel[ch] = hueHist.at<float>(ch) / imageArea;

  return hue;
}

double complementaryChannels(metric::Hue const& hue)
{
  double result = 0;

  for (std::size_t i = 0; i < hue.numberOfChannels; ++i)
    result += hue.channel[i] * hue.channel[(i+hue.numberOfChannels/2) % hue.numberOfChannels];

  return result;
}

}} // namespace phobos::iprocess
