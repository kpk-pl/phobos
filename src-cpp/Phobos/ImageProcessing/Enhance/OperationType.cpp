#include "ImageProcessing/Enhance/OperationType.h"
#include "Utils/Asserted.h"

namespace phobos { namespace iprocess { namespace enhance {

std::string toString(OperationType const operation)
{
  switch (operation)
  {
  case OperationType::AutoWhiteBalance:
    return "white balance";
  }

  return utils::asserted::always;
}

}}} // namespace phobos::iprocess::enhance
