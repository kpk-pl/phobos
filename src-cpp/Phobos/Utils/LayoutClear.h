#ifndef UTILS_LAYOUTCLEAR_H
#define UTILS_LAYOUTCLEAR_H

class QLayout;
class QLayoutItem;

namespace phobos { namespace utils {

void clearLayoutItem(QLayoutItem* item, bool deleteWidgets = true);
void clearLayoutItem(QLayout* layout, int index, bool deleteWidgets = true);
void clearLayout(QLayout* layout, bool deleteWidgets = true);

}} // namespace phobos::utils

#endif // UTILS_LAYOUTCLEAR_H
