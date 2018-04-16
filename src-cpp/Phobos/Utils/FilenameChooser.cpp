#include "Utils/FilenameChooser.h"
#include "PhotoContainers/Set.h"
#include "Dialogs/ConfirmSave.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <easylogging++.h>

namespace phobos { namespace utils {

FilenameChooser::FilenameChooser(pcontainer::Set const& photoSet, QWidget* parent) :
  QObject(parent),
  photoSet(photoSet),
  widgetParent(parent),
  askBeforeOverride(true)
{}

QString FilenameChooser::select(QString const& suggestion)
{
  LOG(TRACE) << "Selecting path starting from " << suggestion;

  QFileInfo info(suggestion);

  QFileDialog dialog(widgetParent, tr("Save as..."));
  dialog.setConfirmOverwrite(false);
  dialog.setDefaultSuffix(info.suffix());

  do
  {
    dialog.setObjectName(info.fileName());
    dialog.setDirectory(info.absoluteDir());

    LOG(TRACE) << "Asking for new filename";
    if (!dialog.exec())
    {
      LOG(TRACE) << "Confirmation canceled";
      return QString{};
    }

    info.setFile(dialog.selectedFiles().front());
    LOG(TRACE) << "Selected " << info.absoluteFilePath();

  } while (!checkPath(info));

  return info.absoluteFilePath();
}

bool FilenameChooser::checkPath(QFileInfo const& fileInfo)
{
  if (photoSet.findItem(fileInfo.absoluteFilePath()))
  {
    QMessageBox::information(widgetParent,
        tr("Invalid file name"),
        tr("Cannot save to this location because this photo is currently imported.\nChoose another location."),
        QMessageBox::StandardButton::Ok,
        QMessageBox::StandardButton::Ok);

    return false;
  }

  if (!askBeforeOverride)
    return true;

  if (!fileInfo.exists())
    return true;

  return askToOverride(fileInfo.absoluteFilePath());
}

bool FilenameChooser::askToOverride(QString const& path)
{
  LOG(TRACE) << "Asking for confirmation";
  dialog::ConfirmSave dialog(path, widgetParent);

  if (dialog.exec() == QDialog::Rejected)
  {
    LOG(TRACE) << "Confirmation canceled";
    return false;
  }

  askBeforeOverride = !dialog.dontAskAgain();
  return true;
}

}} // namespace phobos::utils
