#include "ImageProcessing/Calculator/Bluriness.h"
#include "ImageProcessing/Utils/ROIProcessor.h"
#include <opencv2/opencv.hpp>

namespace phobos { namespace iprocess { namespace calc {

template<>
metric::Blur Bluriness<blur::Sobel>::calculate(cv::Mat const& cvImage) const
{
  constexpr static double scalingValue = 65536.0 / metric::Blur::maximum;

  cv::Mat sobelImg;

  cv::Sobel(cvImage, sobelImg, _depth, 1, 0);
  double const sobelNormX = cv::norm(sobelImg);

  cv::Sobel(cvImage, sobelImg, _depth, 0, 1);
  double const sobelNormY = cv::norm(sobelImg);

  double const sumSq = sobelNormX*sobelNormX + sobelNormY*sobelNormY;
  long const area = cvImage.rows * cvImage.cols;

  double const sobelBlur = sumSq / area; // max at 65536
  return metric::Blur(sobelBlur / scalingValue);
}

template<>
metric::Blur Bluriness<blur::Laplace>::calculate(cv::Mat const& cvImage) const
{
  constexpr static double scalingValue = 16384.0 / metric::Blur::maximum;

  cv::Mat lap;
  cv::Laplacian(cvImage, lap, _depth);
  cv::Scalar mean, stddev;
  cv::meanStdDev(lap, mean, stddev);

  double const laplaceBlur = stddev.val[0] * stddev.val[0]; // max at 16384
  return metric::Blur{laplaceBlur / scalingValue};
}

template<>
metric::Blur Bluriness<blur::LaplaceMod>::calculate(cv::Mat const& cvImage) const
{
  constexpr static double scalingValue = 128.0 / metric::Blur::maximum;

  cv::Mat const kernelX = (cv::Mat_<double>(3, 1) << -1, 2, -1);
  cv::Mat const kernelY = cv::getGaussianKernel(3, -1, CV_64F);

  cv::Mat sepf;
  cv::sepFilter2D(cvImage, sepf, _depth, kernelX, kernelY);
  double const meanX = cv::mean(cv::abs(sepf)).val[0];

  cv::sepFilter2D(cvImage, sepf, _depth, kernelY, kernelX);
  double const meanY = cv::mean(cv::abs(sepf)).val[0];

  double const laplaceModBlur = meanX + meanY; // max at 128
  return metric::Blur{laplaceModBlur / scalingValue};
}

template<typename Strategy>
metric::Blur Bluriness<Strategy>::operator()(cv::Mat const& cvImage) const
{
  if (_n <= 1)
    return calculate(cvImage);

  utils::ROIProcessor<metric::Blur> roiProcessor;
  auto rois = roiProcessor(cvImage, [this](auto const& roi){ return this->calculate(roi); }, _n);
  std::sort(std::begin(rois), std::end(rois));
  return rois.back();
}

template struct Bluriness<blur::Sobel>;
template struct Bluriness<blur::Laplace>;
template struct Bluriness<blur::LaplaceMod>;

}}} // namespace phobos::iprocess::calc
