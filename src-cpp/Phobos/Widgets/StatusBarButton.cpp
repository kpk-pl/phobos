#include "StatusBarButton.h"
#include "ConfigExtension.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"
#include <QSize>

namespace phobos { namespace widgets {

StatusBarButton::StatusBarButton(config::ConfigPath const& path)
{
  QSize const maxSize = config::qSize(path("size"), QSize(15, 15));
  QSize const iconSize = config::qSize(path("iconSize"), QSize(12, 12));

  setIcon(iprocess::utils::coloredPixmap(path, iconSize));

  setFlat(true);
  setFixedSize(maxSize);
}

}} // namespace phobos::widgets
