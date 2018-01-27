#include "ProcessWizard/OperationIcon.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"

namespace phobos { namespace processwiz {

QPixmap operationIcon(OperationType const& operation, QSize const& size)
{
  config::ConfigPath path("operation");

  switch(operation)
  {
  case OperationType::Delete:
    path += "delete";
    break;
  case OperationType::Move:
    path += "move";
    break;
  case OperationType::Copy:
    path += "copy";
    break;
  case OperationType::Rename:
    path += "rename";
    break;
  };

  return iprocess::utils::coloredPixmap(path, size);
}

}} // namespace phobos::processwiz
