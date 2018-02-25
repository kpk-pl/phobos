#ifndef IMAGEPROCESSING_ENHANCE_EXECUTOR_H
#define IMAGEPROCESSING_ENHANCE_EXECUTOR_H

#include "ImageProcessing/Enhance/OperationType.h"
#include <QImage>

namespace phobos { namespace iprocess { namespace enhance {

QImage execute(OperationType const operation, QImage const& source);

}}} // namespace phobos::iprocess::enhance

#endif // IMAGEPROCESSING_ENHANCE_EXECUTOR_H
