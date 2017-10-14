#include "ImageProcessing/Histogram.h"
#include <opencv2/opencv.hpp>

namespace phobos { namespace iprocess {

cv::Mat histogram(cv::Mat const& cvImage, int const histSize)
{
  float range[] = {0, float(histSize)};
  const float* ranges[] = {range};

  cv::Mat hist;
  cv::calcHist(&cvImage, 1, 0, cv::Mat(), hist, 1, &histSize, ranges, true, false);

  return hist;
}

std::vector<float> normalizedHistogram(cv::Mat const& cvImage, double *outContrast)
{
  int const histSize = 256;
  cv::Mat hist = histogram(cvImage, histSize);

  cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

  if (outContrast)
  {
    cv::Scalar mean, stddev;
    cv::meanStdDev(hist, mean, stddev);
    *outContrast = stddev[0];
  }

  std::vector<float> result(histSize, 0);
  for (int i = 0; i<histSize; ++i)
    result[i] = hist.at<float>(i);

  return result;
}

}} // namespace phobos::iprocess
