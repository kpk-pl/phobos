#ifndef UTILS_FILENAMECHOOSER_H
#define UTILS_FILENAMECHOOSER_H

#include "PhotoContainers/Fwd.h"
#include <QString>
#include <QObject>

class QWidget;
class QFileInfo;

namespace phobos { namespace utils {

class FilenameChooser : public QObject
{
Q_OBJECT
public:
  FilenameChooser(pcontainer::Set const& photoSet, QWidget *parent = nullptr);

  QString select(QString const& suggestion);

private:
  bool askToOverride(QString const& path);
  bool checkPath(QFileInfo const& fileInfo);

  pcontainer::Set const& photoSet;
  QWidget* const widgetParent;
  bool askBeforeOverride;
};

}} // namespace phobos::utils

#endif // UTILS_FILENAMECHOOSER_H
