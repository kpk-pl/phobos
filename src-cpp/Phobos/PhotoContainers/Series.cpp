#include "PhotoContainers/Series.h"
#include <easylogging++.h>

namespace phobos { namespace pcontainer {

Series::Series(std::size_t const pOrd) :
  _uuid(QUuid::createUuid()), _ord(pOrd)
{}

Series::Series(std::size_t const pOrd, importwiz::PhotoSeries const& series) :
  Series(pOrd)
{
  for (auto const& item : series)
    addPhotoItem(item.fileName);
}

void Series::addPhotoItem(QString const& fileName)
{
  auto newItem = std::make_shared<Item>(fileName, _uuid, _photoItems.size());
  _photoItems.emplace_back(std::move(newItem));
}

void Series::remove(std::vector<pcontainer::ItemId> const& itemIds)
{
  for (auto itemIt = _photoItems.begin(); itemIt != _photoItems.end(); /* noop */)
  {
    auto const& itemId = (*itemIt)->id();
    if (std::find(itemIds.begin(), itemIds.end(), itemId) != itemIds.end())
    {
      LOG(INFO) << "Removing from series: " << itemId.toString();
      _removedItems.push_back(itemId);
      itemIt = _photoItems.erase(itemIt);
    }
    else
      ++itemIt;
  }
}

}} // namespace phobos::pcontainer
