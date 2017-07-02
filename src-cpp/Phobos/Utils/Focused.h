#ifndef UTILS_FOCUSED_H
#define UTILS_FOCUSED_H

#include <QApplication>
#include "Widgets/PhotoItem/PhotoItem.h"

namespace phobos { namespace utils {

inline widgets::pitem::PhotoItem* focusedPhotoItemWidget()
{
    auto focusWidget = QApplication::focusWidget();
    if (!focusWidget)
        return nullptr;

    widgets::pitem::PhotoItem* piw = dynamic_cast<widgets::pitem::PhotoItem*>(focusWidget);
    return piw;
}

}} // namespace phobos::utils

#endif // UTILS_FOCUSED_H
