#include "PhotoContainers/Series.h"
#include <easylogging++.h>

namespace phobos { namespace pcontainer {

Series::Series() :
  _uuid(QUuid::createUuid())
{}

Series::Series(importwiz::PhotoSeries const& series) :
    Series()
{
  for (auto const& item : series)
    addPhotoItem(item.fileName);
}

void Series::addPhotoItem(QString const& fileName)
{
  auto newItem = std::make_shared<Item>(fileName, _uuid, _photoItems.size());
  _photoItems.emplace_back(std::move(newItem));
}

void Series::remove(QString const& fileName)
{
  auto const fileNameEqual = [&](ItemPtr const& item){ return item->fileName() == fileName; };
  auto const it = std::find_if(_photoItems.begin(), _photoItems.end(), fileNameEqual);
  assert(it != _photoItems.end());

  auto const& itemId = (*it)->id();
  LOG(INFO) << "Removing from series: " << itemId.toString();
  _removedItems.push_back(itemId);
  _photoItems.erase(it);
}

}} // namespace phobos::pcontainer
