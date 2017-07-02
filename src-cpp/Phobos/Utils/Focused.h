#ifndef UTILS_FOCUSED_H
#define UTILS_FOCUSED_H

#include <QApplication>
#include "Widgets/PhotoItemWidget.h"

namespace phobos { namespace utils {

inline widgets::PhotoItemWidget* focusedPhotoItemWidget()
{
    auto focusWidget = QApplication::focusWidget();
    if (!focusWidget)
        return nullptr;

    widgets::PhotoItemWidget* piw = dynamic_cast<widgets::PhotoItemWidget*>(focusWidget);
    return piw;
}

}} // namespace phobos::utils

#endif // UTILS_FOCUSED_H
