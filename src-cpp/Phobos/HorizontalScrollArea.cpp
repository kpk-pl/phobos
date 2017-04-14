#include <QFrame>
#include <QWidget>
#include <QEvent>
#include "HorizontalScrollArea.h"
#include "HeightResizeableInterface.h"

namespace phobos {

HorizontalScrollArea::HorizontalScrollArea(QWidget* parent) :
    QScrollArea(parent)
{
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFrameShape(QFrame::NoFrame);

    _boxLayout = new QHBoxLayout();
    QWidget* scroll = new QWidget();
    scroll->setLayout(_boxLayout);
    setWidget(scroll);
}

bool HorizontalScrollArea::eventFilter(QObject* object, QEvent* event)
{
    if (object == widget() && event->type() == QEvent::Resize)
    {
        widget()->resize(calculateNewSize());
        return true;
    }
    return QScrollArea::eventFilter(object, event);
}

QSize HorizontalScrollArea::calculateNewSize() const
{
    int const height = viewport()->height();

    QMargins const margins = _boxLayout->contentsMargins();
    int const heightForCalc = height - margins.top() - margins.bottom();

    int const width = calcWidthForHeight(heightForCalc);
    return QSize(width, height);
}

int HorizontalScrollArea::calcWidthForHeight(int const height) const
{
    int width = 0;
    for (int i = 0; i < _boxLayout->count(); ++i)
    {
        QWidget* wgt = _boxLayout->itemAt(i)->widget();
        auto const hInt = dynamic_cast<HeightResizeableInterface*>(wgt);
        width += hInt->widthForHeight(height);
    }

    if (_boxLayout->count() > 1)
        width += _boxLayout->spacing() * (_boxLayout->count()-1);

    return width;
}

} // namespace phobos
