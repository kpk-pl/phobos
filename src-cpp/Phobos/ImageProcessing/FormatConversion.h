#ifndef IMAGE_PROCESSING_FORMATCONVERSION_H
#define IMAGE_PROCESSING_FORMATCONVERSION_H

#include <opencv2/core/core.hpp>
#include <QImage>

namespace phobos { namespace iprocess {

QImage convCvToImage(cv::Mat const& cvImage);

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_FORMATCONVERSION_H
