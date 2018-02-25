#include "ImageProcessing/Enhance/AutoWhiteBalance.h"
#include "ImageProcessing/Calculator/Histogram.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <cassert>

namespace phobos { namespace iprocess { namespace enhance {

namespace {
uchar clamp(double l, double lb, double ub)
{
  if (l < lb)
    return lb;

  if (l > ub)
    return ub;

  return l;
}

void processChannel(cv::Mat &channel, double const sideDiscardPercent)
{
  cv::Mat const histogram = calc::grayscaleHistogram(channel);
  assert(histogram.rows == 256);

  int const limit = channel.rows * channel.cols * sideDiscardPercent;

  int lowerBound = 0;
  for (float sum = 0; sum < limit; ++lowerBound)
    sum += histogram.at<float>(lowerBound);

  int upperBound = 255;
  for (float sum = 0; sum < limit; --upperBound)
    sum += histogram.at<float>(upperBound);

  double const boundDifference = upperBound - lowerBound;

  for (int row = channel.rows; row--; )
    for (int col = channel.cols; col--; )
      channel.at<uchar>(row, col) = clamp(double(channel.at<uchar>(row, col)-lowerBound) * 255.0 / boundDifference, 0, 255);
}
} // unnamed namespace

cv::Mat autoWhiteBalance(cv::Mat const& source)
{
  std::vector<cv::Mat> channels;
  cv::split(source, channels);

  for (auto &channel : channels)
    processChannel(channel, 0.05);

  cv::Mat result;
  cv::merge(channels, result);
  return result;
}

}}} // namespace phobos::iprocess::enhance
