#ifndef WIDGETS_CLICKABLELABEL_H
#define WIDGETS_CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>

namespace phobos { namespace widgets {

class ClickableLabel : public QLabel
{
  Q_OBJECT

public:
  explicit ClickableLabel(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
  explicit ClickableLabel(QString const& text, QWidget *parent = nullptr, Qt::WindowFlags f = 0);

signals:
  void clicked();

protected:
  void mousePressEvent(QMouseEvent *event) override;
};

}} // namespace phobos::widgets

#endif // WIDGETS_CLICKABLELABEL_H
