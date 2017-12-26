#include "ImageProcessing/Bluriness.h"
#include "ImageProcessing/ROIProcessor.h"
#include <opencv2/opencv.hpp>

namespace phobos { namespace iprocess {

namespace {
double sobel(cv::Mat const& cvImage, int depth)
{
  cv::Mat sobelImg;

  cv::Sobel(cvImage, sobelImg, depth, 1, 0);
  double const sobelNormX = cv::norm(sobelImg);

  cv::Sobel(cvImage, sobelImg, depth, 0, 1);
  double const sobelNormY = cv::norm(sobelImg);

  double const sumSq = sobelNormX*sobelNormX + sobelNormY*sobelNormY;
  long const area = cvImage.rows * cvImage.cols;
  return sumSq / area;
}

double laplaceMod(cv::Mat const& cvImage, int depth)
{
  cv::Mat const kernelX = (cv::Mat_<double>(3, 1) << -1, 2, -1);
  cv::Mat const kernelY = cv::getGaussianKernel(3, -1, CV_64F);

  cv::Mat sepf;
  cv::sepFilter2D(cvImage, sepf, depth, kernelX, kernelY);
  double const meanX = cv::mean(cv::abs(sepf)).val[0];

  cv::sepFilter2D(cvImage, sepf, depth, kernelY, kernelX);
  double const meanY = cv::mean(cv::abs(sepf)).val[0];

  return meanX + meanY;
}

double laplace(cv::Mat const& cvImage, int depth)
{
  cv::Mat lap;
  cv::Laplacian(cvImage, lap, depth);
  cv::Scalar mean, stddev;
  cv::meanStdDev(lap, mean, stddev);
  return stddev.val[0] * stddev.val[0];
}
} // unnamed namespace

template<>
double Bluriness<blur::Sobel>::calc(cv::Mat const& cvImage) const
{
  return sobel(cvImage, _depth);
}

template<>
double Bluriness<blur::Laplace>::calc(cv::Mat const& cvImage) const
{
  return laplace(cvImage, _depth);
}

template<>
double Bluriness<blur::LaplaceMod>::calc(cv::Mat const& cvImage) const
{
  return laplaceMod(cvImage, _depth);
}

template<typename Strategy>
double Bluriness<Strategy>::operator()(cv::Mat const& cvImage) const
{
  if (_n <= 1)
    return calc(cvImage);

  ROIProcessor<> roiProcessor;
  auto rois = roiProcessor(cvImage, [this](auto const& roi){ return this->calc(roi); }, _n);
  std::sort(std::begin(rois), std::end(rois));
  return rois.back();
}

template struct Bluriness<blur::Sobel>;
template struct Bluriness<blur::Laplace>;
template struct Bluriness<blur::LaplaceMod>;

}} // namespace phobos::iprocess
