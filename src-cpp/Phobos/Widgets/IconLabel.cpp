#include <QHBoxLayout>
#include "Widgets/IconLabel.h"

namespace phobos { namespace widgets {

IconLabel::IconLabel(QString const& text, QWidget *parent) :
    IconLabel(QPixmap(), text, parent)
{
}

IconLabel::IconLabel(QIcon const& icon, QString const& text, QWidget *parent) :
    IconLabel(icon.pixmap(icon.actualSize(QSize(32, 32))), text, parent)
{
}

IconLabel::IconLabel(QPixmap const& icon, QString const& text, QWidget *parent) :
    QFrame(parent), _iconLabel(new QLabel()), _label(new QLabel(text))
{
    _iconLabel->setPixmap(icon);
    _iconLabel->setScaledContents(true);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(_iconLabel);
    hlayout->addWidget(_label);
    setLayout(hlayout);
}

}} // namespace phobos::widgets

