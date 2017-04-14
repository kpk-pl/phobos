#include "ImageProcessing/Histogram.h"
#include <opencv2/opencv.hpp>

namespace phobos { namespace iprocess {

std::vector<float> normalizedHistogram(cv::Mat const& cvImage, double &outContrast)
{
    int histSize = 256;
    float range[] = {0, 256};
    const float* ranges[] = {range};
    cv::Mat hist;

    cv::calcHist(&cvImage, 1, 0, cv::Mat(), hist, 1, &histSize, ranges, true, false);
    cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    cv::Scalar mean, stddev;
    cv::meanStdDev(hist, mean, stddev);
    outContrast = stddev[0];

    double min, max;
    cv::minMaxLoc(hist, &min, &max, nullptr, nullptr);

    std::vector<float> result(histSize, 0);
    for (int i = 0; i<histSize; ++i)
        result[i] = hist.at<float>(i);

    return result;
}

}} // namespace phobos::iprocess
