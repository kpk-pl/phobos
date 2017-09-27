#include "Widgets/IconLabel.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QStyle>
#include <cassert>

namespace phobos { namespace widgets {

IconLabel::IconLabel(QString const& text, QWidget *parent) :
  IconLabel(QPixmap(), text, parent)
{}

IconLabel::IconLabel(QIcon const& icon, QString const& text, QWidget *parent) :
  IconLabel(icon.pixmap(icon.actualSize(QSize(32, 32))), text, parent)
{}

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

namespace {
QStyle::StandardPixmap stdIconEnum(IconLabel::Icon const icon)
{
  switch(icon)
  {
  case IconLabel::Icon::Critical:
    return QStyle::SP_MessageBoxCritical;
  case IconLabel::Icon::Information:
    return QStyle::SP_MessageBoxInformation;
  case IconLabel::Icon::Question:
    return QStyle::SP_MessageBoxQuestion;
  case IconLabel::Icon::Warning:
    return QStyle::SP_MessageBoxWarning;
  }

  assert(false);
  return QStyle::SP_MessageBoxCritical;
}
} // unnamed namespace

IconLabel::IconLabel(Icon const icon, QString const& text, QWidget *parent) :
  IconLabel(QApplication::style()->standardIcon(stdIconEnum(icon)), text, parent)
{}

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

TextIconLabel::TextIconLabel(Icon const& icon, QString const& text, QWidget *parent) :
  TextIconLabel(QApplication::style()->standardIcon(stdIconEnum(icon)), text, parent)
{}

TextIconLabel::TextIconLabel(QPixmap const& icon, QString const& text, QWidget *parent) :
  IconLabel(icon, text, parent)
{
  _iconLabel->setFixedSize(16, 16);
}

}} // namespace phobos::widgets

