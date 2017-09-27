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
  enum class Icon { Critical, Information, Question, Warning };

  IconLabel(QString const& text = "", QWidget *parent = nullptr);
  IconLabel(QPixmap const& icon, QString const& text = "", QWidget *parent = nullptr);
  IconLabel(QIcon const& icon, QString const& text = "", QWidget *parent = nullptr);
  IconLabel(Icon const icon, QString const& text = "", QWidget *parent = nullptr);

  QLabel* iconLabel() const { return _iconLabel; }
  QLabel* label() const { return _label; }

protected:
  QLabel *_iconLabel;
  QLabel *_label;
};

class TextIconLabel : public IconLabel
{
public:
  TextIconLabel(QString const& text = "", QWidget *parent = nullptr);
  TextIconLabel(QPixmap const& icon, QString const& text = "", QWidget *parent = nullptr);
  TextIconLabel(QIcon const& icon, QString const& text = "", QWidget *parent = nullptr);
  TextIconLabel(Icon const& icon, QString const& text = "", QWidget *parent = nullptr);
};

}} // namespace phobos::widgets

#endif // WIDGETS_ICONLABEL_H_
