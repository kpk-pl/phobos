#include "PhotoContainers/Set.h"
#include "Utils/Comparators.h"

namespace phobos { namespace pcontainer {

SeriesPtr const& Set::front() const
{
    assert(!_photoSeries.empty());
    return _photoSeries.front();
}

void Set::addSeries(importwiz::PhotoSeriesVec const& newPhotoSeries)
{
    for (auto const& series : newPhotoSeries)
    {
        SeriesPtr seriesPtr = std::make_shared<Series>(series);
        _photoSeries.push_back(seriesPtr);
        emit newSeries(seriesPtr);
    }
}

SeriesPtr const& Set::findSeries(QUuid const& seriesUuid) const
{
  auto const uuidEqual = [&](SeriesPtr const& series){ return series->uuid() == seriesUuid; };
  auto const it = std::find_if(_photoSeries.begin(), _photoSeries.end(), uuidEqual);
  assert(it != _photoSeries.end());
  return *it;
}

SeriesPtr const& Set::findSeries(QUuid const& seriesUuid,
                                 int offset) const
{
    for (std::size_t i = 0; i<_photoSeries.size(); ++i)
        if (_photoSeries[i]->uuid() == seriesUuid)
            return _photoSeries[(int(i)+offset) % _photoSeries.size()];

    assert(false);
    return _photoSeries.front(); // dummy
}

void Set::removeImage(pcontainer::ItemId const& itemId)
{
  removeImagesImpl({itemId});
}

void Set::removeImages(std::vector<pcontainer::ItemId> itemIds)
{
  std::sort(itemIds.begin(), itemIds.end(), utils::less().on(&ItemId::seriesUuid));

  for (auto it = itemIds.begin(); it != itemIds.end(); /* noop */)
  {
    auto const lookedUuid = it->seriesUuid;
    auto partIt = std::find_if_not(it, itemIds.end(), [lookedUuid](auto const& id){return id.seriesUuid == lookedUuid;});
    std::vector<pcontainer::ItemId> fromSeries(it, partIt);

    removeImagesImpl(fromSeries);
    it = partIt;
  }
}

void Set::removeImagesImpl(std::vector<pcontainer::ItemId> const& itemIds)
{
  if (itemIds.empty())
    return;

  SeriesPtr const& series = findSeries(itemIds.front().seriesUuid);
  series->remove(itemIds);
  // possible that this series remains empty forever

  emit changedSeries(series->uuid());
}

}} // namespace phobos::pcontainer
