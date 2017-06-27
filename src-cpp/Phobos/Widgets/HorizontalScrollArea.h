#ifndef PHOBOS_WIDGETS_HORIZONTALSCROLLAREA_H
#define PHOBOS_WIDGETS_HORIZONTALSCROLLAREA_H

#include <QScrollArea>
#include <QHBoxLayout>

namespace phobos { namespace widgets {

class HorizontalScrollArea : public QScrollArea
{
public:
    explicit HorizontalScrollArea(QWidget* parent = nullptr);

    bool eventFilter(QObject* object, QEvent* event) override;

    QHBoxLayout* boxLayout() const { return _boxLayout; }

private:
    QSize calculateNewSize() const;
    int calcWidthForHeight(int const height) const;

    QHBoxLayout* _boxLayout;
};

}} // namespace phobos::widgets

#endif // PHOBOS_WIDGETS_HORIZONTALSCROLLAREA_H
