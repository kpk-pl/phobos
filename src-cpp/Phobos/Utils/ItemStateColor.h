#ifndef UTILS_ITEMSTATECOLOR_H
#define UTILS_ITEMSTATECOLOR_H

#include "PhotoContainers/ItemState.h"
#include <QColor>

namespace phobos { namespace utils {

QColor itemStateColor(pcontainer::ItemState const& state);

}} // namespace phobos::utils

#endif // UTILS_ITEMSTATECOLOR_H
