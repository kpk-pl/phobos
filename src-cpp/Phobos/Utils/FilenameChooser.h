#ifndef UTILS_FILENAMECHOOSER_H
#define UTILS_FILENAMECHOOSER_H

#include <QString>
#include <QObject>

class QWidget;

namespace phobos { namespace utils {

class FilenameChooser : public QObject
{
Q_OBJECT
public:
  FilenameChooser(QWidget *parent = nullptr);

  QString confirm(QString const& path);
  QString select(QString const& suggestion);

private:
  bool askToOverride(QString const& path, bool& askAgain) const;

  QWidget* const widgetParent;
  bool askBeforeOverrideSave;
  bool askBeforeOverrideSaveAs;
};

}} // namespace phobos::utils

#endif // UTILS_FILENAMECHOOSER_H
