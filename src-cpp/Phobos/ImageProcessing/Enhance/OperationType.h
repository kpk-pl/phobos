#ifndef IMAGEPROCESSING_ENHANCE_OPERATIONTYPE_H
#define IMAGEPROCESSING_ENHANCE_OPERATIONTYPE_H

#include <string>

namespace phobos { namespace iprocess { namespace enhance {

enum class OperationType
{
  AutoWhiteBalance
};

std::string toString(OperationType const operation);

}}} // namespace phobos::iprocess::enhance

#endif // IMAGEPROCESSING_ENHANCE_OPERATIONTYPE_H
