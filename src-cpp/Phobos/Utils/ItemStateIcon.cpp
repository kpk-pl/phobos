#include "Utils/ItemStateIcon.h"
#include "Utils/LexicalCast.h"
#include "ImageProcessing/ColoredPixmap.h"
#include "Config.h"
#include "ConfigPath.h"
#include "ConfigExtension.h"
#include <easylogging++.h>
#include <cassert>

namespace phobos { namespace utils {

QPixmap itemStateIcon(pcontainer::ItemState const& state, QSize const& size)
{
  auto const configPath = config::ConfigPath("category")(utils::lexicalCast(state));
  auto const path = config::qualified(configPath("icon"), std::string{});
  auto const color = config::qColor(configPath("color"), Qt::black);

  return iprocess::coloredPixmap(path, color, size);
}

}} // namespace phobos::utils
