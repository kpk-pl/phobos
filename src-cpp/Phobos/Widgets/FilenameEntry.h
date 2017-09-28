#ifndef WIDGETS_FILENAME_ENTRY_H_
#define WIDGETS_FILENAME_ENTRY_H_

#include <QWidget>
#include <QString>

class QLineEdit;
class QHBoxLayout;

namespace phobos { namespace widgets {

class TextIconLabel;

class FilenameEntry : public QWidget
{
  Q_OBJECT

public:
  FilenameEntry(std::string const& unequivocalFlags, char const defaultFlag);

  void setSideWidget(QWidget *widget);

  QString unequivocalSyntax() const;
  QLineEdit *fileNameEdit;

signals:
  void helpRequested();

private slots:
  void updateLabels() const;

private:
  std::string const unequivocalFlags;
  char const defaultFlag;

  bool isAmbiguous() const;

  QHBoxLayout *labelAndSideWgtLayout;
  widgets::TextIconLabel *prependInfo, *incorrectWrn;
};

}} // namespace phobos::widgets

#endif // WIDGETS_FILENAME_ENTRY_H_
