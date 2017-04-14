#ifndef UTILS_LAYOUTCLEAR_H
#define UTILS_LAYOUTCLEAR_H

class QLayout;

namespace phobos { namespace utils {

void clearLayout(QLayout* layout, bool deleteWidgets = true);

}} // namespace phobos::utils

#endif // UTILS_LAYOUTCLEAR_H
