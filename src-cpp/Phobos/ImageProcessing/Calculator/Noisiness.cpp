#include "ImageProcessing/Calculator/Noisiness.h"
#include <opencv2/opencv.hpp>
#include <cmath>

namespace phobos { namespace iprocess { namespace calc {

metric::Noise noiseMeasure(cv::Mat const& cvImage, int medianSize)
{
  cv::Mat med;
  cv::medianBlur(cvImage, med, medianSize);
  return metric::Noise{std::log2(cv::mean(cvImage-med)[0] + 1.0)};
}

}}} // namespace phobos::iprocess::calc
