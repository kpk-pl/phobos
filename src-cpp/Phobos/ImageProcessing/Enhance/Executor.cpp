#include "ImageProcessing/Enhance/Executor.h"
#include "ImageProcessing/Enhance/AutoWhiteBalance.h"
#include "ImageProcessing/Utils/FormatConversion.h"
#include "Utils/Asserted.h"
#include "ConfigExtension.h"
#include <easylogging++.h>

namespace phobos { namespace iprocess { namespace enhance {

QImage execute(OperationType const operation, QImage const& source)
{
  TIMED_FUNC_IF(timer, config::qualified("logging.enhancements", false));

  switch(operation)
  {
  case OperationType::AutoWhiteBalance:
    return utils::convCvToImage(autoWhiteBalance(utils::convImageToCv(source, false)));
  }

  return phobos::utils::asserted::always;
}

}}} // namespace phobos::iprocess::enhance
