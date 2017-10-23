#include "ImportWizard/FileInfoProvider.h"
#include "PhotoContainers/FileInfo.h"
#include <easylogging++.h>

namespace phobos { namespace importwiz {

std::vector<Photo> provideFileInfo(QStringList const& photos)
{
  TIMED_SCOPE(scopeFunc, "provideFileInfo");

  std::vector<Photo> result;
  result.reserve(photos.size());

  std::transform(photos.begin(), photos.end(), std::back_inserter(result),
      [](QString const& str) -> Photo {
        return {str, pcontainer::FileInfo(str)};
      });

  return result;
}

}} // namespace phobos::importwiz
