#include "Utils/Filesystem/Trash.h"
#include <easylogging++.h>

/*
 * Code from https://stackoverflow.com/questions/17964439/move-files-to-trash-recycle-bin-in-qt
 * Thanks to Reto ODT
 */

#ifdef _WIN32
#include <QFileInfo>
#include <windows.h>
#include <cstring>
#include <cassert>

namespace {
bool trashAvailableImpl()
{
  return true;
}

bool moveToTrashImpl(QString const& file)
{
  QFileInfo const fileinfo(file);

  if(!fileinfo.exists())
  {
    LOG(ERROR) << "Cannot move to trash (file \"" << file << "\" does not exist)";
    return false;
  }

  WCHAR from[MAX_PATH];
  std::memset(from, 0, sizeof(from));

  int const len = fileinfo.absoluteFilePath().toWCharArray(from);
  assert(0 <= len && len < MAX_PATH);
  from[len] = '\0';

  SHFILEOPSTRUCT fileop;
  std::memset(&fileop, 0, sizeof(fileop));

  fileop.wFunc = FO_DELETE;
  fileop.pFrom = from;
  fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

  int const rv = SHFileOperation( &fileop );
  if(rv != 0) {
    LOG(ERROR) << "Failed move to trash (file \"" << file << "\")";
    LOG(DEBUG) << "Move to trash failed: file: \"" << file << "\" errno: " << rv;
    return false;
  }

  return true;
}
} // unnamed namespace

#elif __linux__
#include <QStringList>
#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QTextStream>

namespace {
#ifdef QT_GUI_LIB
bool TrashIsAvailable = false;
bool TrashInitialized = false;
QString TrashPath;
QString TrashPathInfo;
QString TrashPathFiles;

void initializeTrash()
{
  QStringList paths;
  const char* xdg_data_home = getenv("XDG_DATA_HOME");
  if(xdg_data_home)
  {
    LOG(DEBUG) << "XDG_DATA_HOME not yet tested";
    QString xdgTrash(xdg_data_home);
    paths.append(xdgTrash + "/Trash");
  }

  QString const home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  paths.append(home + "/.local/share/Trash");
  paths.append(home + "/.trash");

  for(auto const& path : paths)
    if(TrashPath.isEmpty())
    {
      QDir dir(path);
      if(dir.exists())
        TrashPath = path;
    }

  if(TrashPath.isEmpty())
  {
    TrashInitialized = true;
    TrashIsAvailable = false;
    return;
  }

  TrashPathInfo = TrashPath + "/info";
  TrashPathFiles = TrashPath + "/files";

  if(!QDir(TrashPathInfo).exists() || !QDir(TrashPathFiles).exists())
    LOG(DEBUG) << "Trash doesn't looks like FreeDesktop.org Trash specification";

  TrashInitialized = true;
  TrashIsAvailable = true;
}

bool trashAvailableImpl()
{
  if(!TrashInitialized)
    initializeTrash();

  return TrashIsAvailable;
}

bool moveToTrashImpl(QString const& file)
{
  if(!TrashInitialized)
    initializeTrash();

  QFileInfo const original(file);
  if(!original.exists())
  {
    LOG(ERROR) << "Cannot move to trash (file \"" << file << "\" does not exist)";
    return false;
  }

  QString info;
  info += "[Trash Info]\nPath=";
  info += original.absoluteFilePath();
  info += "\nDeletionDate=";
  info += QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzzZ");
  info += "\n";

  QString trashname = original.fileName();
  QString infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
  QString filepath = TrashPathFiles + "/" + trashname;

  int nr = 1;
  while(QFileInfo(infopath ).exists() || QFileInfo(filepath).exists())
  {
    nr++;
    trashname = original.baseName() + "." + QString::number(nr);

    if(!original.completeSuffix().isEmpty())
      trashname += QString( "." ) + original.completeSuffix();

    infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
    filepath = TrashPathFiles + "/" + trashname;
  }

  QDir dir;
  if(!dir.rename(original.absoluteFilePath(), filepath))
  {
    LOG(ERROR) << "Failed move to trash (file \"" << file << "\")";
    LOG(DEBUG) << "Move to trash failed: file: \"" << file << "\" filepath: " << filepath;
    return false;
  }

  QFile infofile(infopath);
	if (!infofile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
    LOG(DEBUG) << "Cannot open infofile " << infopath << " for writing";
    return true;
	}

	QTextStream streamFileOut(&infofile);
  streamFileOut.setGenerateByteOrderMark(true);
	streamFileOut.setCodec("UTF-8");
	streamFileOut << info;
	streamFileOut.flush();

  infofile.close();
  return true;
}

#else // QT_GUI_LIB
bool trashAvailableImpl()
  return false;
}

bool moveToTrashImpl()
  return false;
}
#endif

} // unnamed namespace

#endif

namespace phobos { namespace utils { namespace fs {

bool trashAvailable()
{
  return trashAvailableImpl();
}

bool moveToTrash(QString const& file)
{
  return moveToTrashImpl(file);
}

}}} // namespace phobos::utils::fs
