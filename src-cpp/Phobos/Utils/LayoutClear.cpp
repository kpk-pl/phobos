#include <cassert>
#include <QLayout>
#include <QWidget>
#include "Utils/LayoutClear.h"

namespace phobos { namespace utils {

void clearLayout(QLayout* layout, bool deleteWidgets)
{
    while(true)
    {
        auto item = layout->takeAt(0);
        if (!item)
            break;

        QWidget* widget = item->widget();
        if (widget)
        {
            widget->setParent(nullptr);
            if (deleteWidgets)
                delete widget;
        }

        QLayout* insideLayout = item->layout();
        if (insideLayout)
        {
            clearLayout(insideLayout, deleteWidgets);
            insideLayout->setParent(nullptr);
            if (deleteWidgets)
                delete insideLayout;
        }

        delete item;
    }
    assert(layout->count() == 0);
}

}} // namespace phobos::utils
