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
  FilenameEntry(std::string const& unequivocalFlags, char const defaultFlag);

  QString unequivocalSyntax() const;
  QLineEdit *fileNameEdit;

private slots:
  void updateLabels() const;

private:
  std::string const unequivocalFlags;
  char const defaultFlag;

  bool isAmbiguous() const;

  widgets::TextIconLabel *prependInfo, *incorrectWrn;
};

}} // namespace phobos::widgets

#endif // WIDGETS_FILENAME_ENTRY_H_
