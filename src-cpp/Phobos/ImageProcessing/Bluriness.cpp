#include "ImageProcessing/Bluriness.h"
#include <opencv2/opencv.hpp>

namespace phobos { namespace iprocess { namespace blur {

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

}}} // namespace phobos::iprocess::blur
