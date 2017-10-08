#include "ImageProcessing/Sharpness.h"
#include "ImageProcessing/Histogram.h"
#include <opencv2/opencv.hpp>
#include <cassert>
#include <set>

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
} // unnamed namespace

double homogeneous(cv::Mat const& cvImage, int const kernel)
{
  cv::Mat temp;
  cv::blur(cvImage, temp, cv::Size(kernel, kernel), cv::Point(-1, -1));
  temp = cvImage - temp;
  return nthPercentile(temp, 0.98);
}

double gaussian(cv::Mat const& cvImage, int const kernel)
{
  cv::Mat temp;
  cv::GaussianBlur(cvImage, temp, cv::Size(kernel, kernel), 0, 0);
  temp = cvImage - temp;
  return nthPercentile(temp, 0.98);
}

}}} // namespace phobos::iprocess::sharpness
