#include "ImageProcessing/Calculator/Saturation.h"

namespace phobos { namespace iprocess { namespace calc {

metric::Saturation saturation(cv::Mat const& hsvImage)
{
  return metric::Saturation{cv::mean(hsvImage)[1]};
}

}}} // namespace phobos::iprocess::calc
