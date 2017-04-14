#ifndef HORIZONTALSCROLLAREA_H
#define HORIZONTALSCROLLAREA_H

#include <QScrollArea>
#include <QHBoxLayout>

namespace phobos {

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

} // namespace phobos

#endif // HORIZONTALSCROLLAREA_H
