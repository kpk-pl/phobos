#ifndef UTILS_ITEMSTATECOLOR_H
#define UTILS_ITEMSTATECOLOR_H

#include "PhotoContainers/ItemState.h"
#include <QPixmap>
#include <QSize>

namespace phobos { namespace utils {

QPixmap itemStateIcon(pcontainer::ItemState const& state, QSize const& size = QSize{});

}} // namespace phobos::utils

#endif // UTILS_ITEMSTATECOLOR_H
