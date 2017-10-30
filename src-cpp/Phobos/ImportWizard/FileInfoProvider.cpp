#include "ImportWizard/FileInfoProvider.h"
#include "PhotoContainers/FileInfo.h"
#include <easylogging++.h>
#include <QProgressDialog>

namespace phobos { namespace importwiz {

std::vector<Photo> provideFileInfo(QStringList const& photos, QWidget *parent)
{
  LOG(INFO) << "Importing " << photos.size() << " new files";
  TIMED_SCOPE(scopeFunc, "provideFileInfo");

  std::vector<Photo> result;
  result.reserve(photos.size());

  QProgressDialog progress(QObject::tr("Importing files..."), QObject::tr("Cancel"), 0, photos.size(), parent);
  progress.setWindowModality(Qt::WindowModal);

  for (auto const& photoPath : photos)
  {
    progress.setValue(progress.value() + 1);
    if (progress.wasCanceled())
    {
      LOG(INFO) << "Import was canceled by the user";
      result.clear();
      break;
    }

    result.emplace_back(Photo{photoPath, pcontainer::FileInfo(photoPath)});
  }

  progress.setValue(progress.maximum());

  LOG(INFO) << "Returning " << result.size() << " imported photos";
  return result;
}

}} // namespace phobos::importwiz
