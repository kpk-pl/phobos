#ifndef WIDGETS_ICONLABEL_H_
#define WIDGETS_ICONLABEL_H_

#include <QLabel>
#include <QPixmap>
#include <QWidget>
#include <QIcon>
#include <QString>
#include <QFrame>

namespace phobos { namespace widgets {

class IconLabel : public QFrame
{
public:
    IconLabel(QString const& text = "", QWidget *parent = nullptr);
    IconLabel(QPixmap const& icon, QString const& text = "", QWidget *parent = nullptr);
    IconLabel(QIcon const& icon, QString const& text = "", QWidget *parent = nullptr);

    QLabel* iconLabel() const { return _iconLabel; }
    QLabel* label() const { return _label; }

protected:
    QLabel *_iconLabel;
    QLabel *_label;

};

}} // namespace phobos::widgets

#endif // WIDGETS_ICONLABEL_H_
