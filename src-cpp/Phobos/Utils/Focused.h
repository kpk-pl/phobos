#ifndef UTILS_FOCUSED_H
#define UTILS_FOCUSED_H

#include <QApplication>
#include "PhotoItemWidget.h"

namespace phobos { namespace utils {

inline PhotoItemWidget* focusedPhotoItemWidget()
{
    auto focusWidget = QApplication::focusWidget();
    if (!focusWidget)
        return nullptr;

    PhotoItemWidget* piw = dynamic_cast<PhotoItemWidget*>(focusWidget);
    return piw;
}

}} // namespace phobos::utils

#endif // UTILS_FOCUSED_H
