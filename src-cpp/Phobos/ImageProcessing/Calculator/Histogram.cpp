#include "ImageProcessing/Calculator/Histogram.h"
#include <opencv2/opencv.hpp>

namespace phobos { namespace iprocess { namespace calc {

cv::Mat grayscaleHistogram(cv::Mat const& cvImage)
{
  int bins[] = { 256 };
  float rangeArray[] = {0.0, 256.0};
  const float* ranges[] = {rangeArray};
  int channels[] = {0};

  cv::Mat hist;
  cv::calcHist(&cvImage, 1, channels, cv::Mat(), hist, 1, bins, ranges);

  return hist;
}

std::vector<float> normalizedHistogram(cv::Mat const& cvImage, double *outContrast)
{
  int const histSize = 256;
  cv::Mat hist = grayscaleHistogram(cvImage);

  // Limit scale to 0-1 max in each bin
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

}}} // namespace phobos::iprocess::calc
