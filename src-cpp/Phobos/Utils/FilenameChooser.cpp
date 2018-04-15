#include "Utils/FilenameChooser.h"
#include "Dialogs/ConfirmSave.h"
#include <QFileDialog>
#include <QFileInfo>
#include <easylogging++.h>

namespace phobos { namespace utils {

FilenameChooser::FilenameChooser(QWidget* parent) :
  QObject(parent), widgetParent(parent), askBeforeOverrideSave(true), askBeforeOverrideSaveAs(true)
{}

QString FilenameChooser::confirm(QString const& path)
{
  LOG(TRACE) << "Confirming path " << path;

  if (askBeforeOverrideSave && !askToOverride(path, askBeforeOverrideSave))
    return QString{};

  LOG(TRACE) << "Confirmed " << path;
  return path;
}

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

  } while (askBeforeOverrideSaveAs && info.exists() && !askToOverride(info.absoluteFilePath(), askBeforeOverrideSaveAs));

  return info.absoluteFilePath();
}

bool FilenameChooser::askToOverride(QString const& path, bool& askAgain) const
{
  LOG(TRACE) << "Asking for confirmation";
  dialog::ConfirmSave dialog(path, widgetParent);

  if (dialog.exec() == QDialog::Rejected)
  {
    LOG(TRACE) << "Confirmation canceled";
    return false;
  }

  askAgain = !dialog.dontAskAgain();
  return true;
}

}} // namespace phobos::utils
