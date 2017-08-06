#include "PhotoContainers/Series.h"
#include <easylogging++.h>

namespace phobos { namespace pcontainer {

Series::Series() :
    _uuid(QUuid::createUuid())
{
}

Series::Series(importwiz::PhotoSeries const& series) :
    Series()
{
    for (auto const& item : series)
        addPhotoItem(item.fileName);
}

void Series::addPhotoItems(std::vector<std::string> const& fileNames)
{
    photoItems.reserve(photoItems.size() + fileNames.size());
    for (auto const& fn : fileNames)
        addPhotoItem(fn);
}

void Series::addPhotoItem(std::string const& fileName)
{
    auto newItem = std::make_shared<Item>(fileName, _uuid, photoItems.size());
    photoItems.emplace_back(std::move(newItem));
}

void Series::remove(QString const& fileName)
{
  auto const fileNameEqual = [&](ItemPtr const& item){ return item->fileName() == fileName; };
  auto const it = std::find_if(photoItems.begin(), photoItems.end(), fileNameEqual);
  assert(it != photoItems.end());

  LOG(INFO) << "Removing from series: " << (*it)->id().toString();
  photoItems.erase(it, it+1);
}

}} // namespace phobos::pcontainer
