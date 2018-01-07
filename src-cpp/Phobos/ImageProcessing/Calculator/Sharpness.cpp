#include "ImageProcessing/Calculator/Sharpness.h"
#include "ImageProcessing/Calculator/Histogram.h"
#include "ImageProcessing/Utils/ROIProcessor.h"
#include <opencv2/opencv.hpp>
#include <cassert>
#include <set>
#include <array>
#include <algorithm>

namespace phobos { namespace iprocess { namespace calc {

namespace {
double nthPercentile(cv::Mat const& cvImage, float const n)
{
  assert(cvImage.depth() == CV_8U);
  assert(cvImage.channels() == 1);

  long const area = cvImage.rows * cvImage.cols;
  long const nthPercArea = n*area;

  cv::Mat const hist = grayscaleHistogram(cvImage);

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

inline cv::Mat simpleHighImage(cv::Mat const& cvImage, int const kernel)
{
  cv::Mat temp;
  cv::blur(cvImage, temp, cv::Size(kernel, kernel), cv::Point(-1, -1));
  return cvImage - temp;
}

inline cv::Mat gaussianHighImage(cv::Mat const& cvImage, int const kernel)
{
  cv::Mat temp;
  cv::GaussianBlur(cvImage, temp, cv::Size(kernel, kernel), 0, 0);
  return cvImage - temp;
}
} // unnamed namespace

template<>
Sharpness<sharpness::Homogeneous>::Sharpness(cv::Mat const& cvImage, int const kernel) :
  highImage(simpleHighImage(cvImage, kernel))
{}

template<>
Sharpness<sharpness::Gaussian>::Sharpness(cv::Mat const& cvImage, int const kernel) :
  highImage(gaussianHighImage(cvImage, kernel))
{}

template<typename Strategy>
metric::Sharpness Sharpness<Strategy>::sharpness() const
{
  return metric::Sharpness{nthPercentile(highImage, percentile)};
}

template<typename Strategy>
metric::DepthOfField Sharpness<Strategy>::depthOfField() const
{
  utils::ROIProcessor<> roiProcessor;
  auto const segmentSharpness = roiProcessor(highImage,
      [this](auto const& roi){
        return nthPercentile(roi, percentile);
      }, 5);

  auto const raws = utils::aggregateROI(std::move(segmentSharpness));
  return metric::DepthOfField(std::get<0>(raws), std::get<1>(raws), std::get<2>(raws));
}

template struct Sharpness<sharpness::Homogeneous>;
template struct Sharpness<sharpness::Gaussian>;

}}} // namespace phobos::iprocess::calc
