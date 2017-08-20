#ifndef WIDGETS_FILENAME_ENTRY_H_
#define WIDGETS_FILENAME_ENTRY_H_

#include <QWidget>
#include <QString>

class QLineEdit;

namespace phobos { namespace widgets {

class TextIconLabel;

class FilenameEntry : public QWidget
{
  Q_OBJECT

public:
  FilenameEntry();

  QString unequivocalSyntax() const;
  QLineEdit *fileNameEdit;

private slots:
  void updateLabels() const;

private:
  widgets::TextIconLabel *prependInfo, *incorrectWrn;
};

}} // namespace phobos::widgets

#endif // WIDGETS_FILENAME_ENTRY_H_
