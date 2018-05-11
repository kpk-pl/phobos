#include "PhotoContainers/Series.h"
#include <easylogging++.h>

namespace phobos { namespace pcontainer {

Series::Series(std::size_t const pOrd, importwiz::PhotoSeries const& series) :
  isPhotoSeries(series.isASeries), _uuid(QUuid::createUuid()), _ord(pOrd)
{
  for (auto const& item : series)
    addPhotoItem(item);
}

void Series::addPhotoItem(importwiz::Photo const& importPhoto)
{
  auto newItem = std::make_shared<Item>(importPhoto, _uuid, _photoItems.size());
  _photoItems.emplace_back(std::move(newItem));
}

void Series::remove(std::vector<pcontainer::ItemId> const& itemIds)
{
  for (auto itemIt = _photoItems.begin(); itemIt != _photoItems.end(); /* noop */)
  {
    auto const& itemId = (*itemIt)->id();
    if (std::find(itemIds.begin(), itemIds.end(), itemId) != itemIds.end())
    {
      LOG(TRACE) << "Removing from series: " << itemId.toString();
      _removedItems.push_back(itemId);
      itemIt = _photoItems.erase(itemIt);
    }
    else
      ++itemIt;
  }
}

void Series::removeAll()
{
  LOG(TRACE) << "Removing all series: " << _uuid.toString();

  for (auto const& item : _photoItems)
  {
    _removedItems.push_back(item->id());
    LOG(DEBUG) << "Removing from series: " << item->id().toString();
  }

  _photoItems.clear();
}

}} // namespace phobos::pcontainer
