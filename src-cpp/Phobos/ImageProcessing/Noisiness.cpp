#include "ImageProcessing/Noisiness.h"
#include <opencv2/opencv.hpp>

namespace phobos { namespace iprocess {

double noiseMeasure(cv::Mat const& cvImage, int medianSize)
{
  cv::Mat med;
  cv::medianBlur(cvImage, med, medianSize);
  return cv::mean(cvImage-med)[0];
}

}}
