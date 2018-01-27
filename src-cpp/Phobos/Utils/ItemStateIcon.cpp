#include "Utils/ItemStateIcon.h"
#include "Utils/LexicalCast.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"
#include "Config.h"
#include "ConfigPath.h"
#include "ConfigExtension.h"
#include <easylogging++.h>
#include <cassert>

namespace phobos { namespace utils {

QPixmap itemStateIcon(pcontainer::ItemState const& state, QSize const& size)
{
  auto const configPath = config::ConfigPath("category")(utils::lexicalCast(state));
  return iprocess::utils::coloredPixmap(configPath, size);
}

}} // namespace phobos::utils
