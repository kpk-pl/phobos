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
    hlayout->setContentsMargins(0, 0, 0, 0);

    hlayout->addWidget(_iconLabel);
    hlayout->addWidget(_label);
    setLayout(hlayout);
}

TextIconLabel::TextIconLabel(QString const& text, QWidget *parent) :
  IconLabel(text, parent)
{
  _iconLabel->setFixedSize(16, 16);
}

TextIconLabel::TextIconLabel(QIcon const& icon, QString const& text, QWidget *parent) :
  IconLabel(icon, text, parent)
{
  _iconLabel->setFixedSize(16, 16);
}

TextIconLabel::TextIconLabel(QPixmap const& icon, QString const& text, QWidget *parent) :
  IconLabel(icon, text, parent)
{
  _iconLabel->setFixedSize(16, 16);
}

}} // namespace phobos::widgets

