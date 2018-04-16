#include "PhotoContainers/Set.h"
#include "Utils/Comparators.h"
#include "Utils/Asserted.h"
#include <easylogging++.h>

namespace phobos { namespace pcontainer {

Series const& Set::front() const
{
  return utils::asserted::fromPtr(_photoSeries.front());
}

void Set::addSeries(importwiz::PhotoSeriesVec const& newPhotoSeries)
{
  LOG(TRACE) << "Adding " << newPhotoSeries.size() << " new series to set";
  for (auto const& series : newPhotoSeries)
  {
    SeriesPtr seriesPtr = std::make_shared<Series>(_photoSeries.size(), series);
    _photoSeries.push_back(seriesPtr);
    LOG(DEBUG) << "Added series #" << seriesPtr->ord() << " " << seriesPtr->uuid().toString();
    emit newSeries(seriesPtr);
  }
}

bool Set::hasPhotos() const
{
  return std::any_of(_photoSeries.begin(), _photoSeries.end(), [](SeriesPtr const& s){ return !s->empty(); });
}

namespace {
  std::size_t posModulo(int val, int mod)
  {
    val %= mod;
    if (val < 0)
      val += mod;
    return val;
  }
  int sign(int val)
  {
    return val < 0 ? -1 : 1;
  }
}

SeriesPtr const& Set::findSeriesImpl(QUuid const& seriesUuid, int const offset) const
{
  for (std::size_t i = 0; i<_photoSeries.size(); ++i)
    if (_photoSeries[i]->uuid() == seriesUuid)
      return _photoSeries[posModulo(int(i)+offset, _photoSeries.size())];

  return utils::asserted::always;
}

Series const& Set::findSeries(QUuid const& seriesUuid, int const offset) const
{
  return utils::asserted::fromPtr(findSeriesImpl(seriesUuid, offset));
}

Series const& Set::findNonEmptySeries(QUuid const& seriesUuid, int const offset) const
{
  return nonEmpty(findSeriesImpl(seriesUuid, offset)->ord());
}

Series const& Set::nonEmpty(std::size_t const n, int const offset) const
{
  std::size_t const startIdx = posModulo(int(n)+offset, _photoSeries.size());
  std::size_t searchIdx = startIdx;
  do
  {
    SeriesPtr const& series = _photoSeries[searchIdx];
    if (!series->empty())
      return *series;
    searchIdx = posModulo(int(searchIdx) + sign(offset), _photoSeries.size());
  } while (searchIdx != startIdx);

  return utils::asserted::always;
}

Series const& Set::lastNonEmpty() const
{
  for (auto it = _photoSeries.rbegin(); it != _photoSeries.rend(); ++it)
    if (!(*it)->empty())
      return **it;

  return utils::asserted::always;
}

ItemPtr Set::findItem(ItemId const& id) const
{
  SeriesPtr const series = findSeriesImpl(id.seriesUuid, 0);
  if (!series)
    return nullptr;

  auto const it = std::find_if(series->begin(), series->end(), [&id](ItemPtr const& item){ return item->id() == id; });
  if (it == series->end())
    return nullptr;

  return *it;
}

ItemPtr Set::findItem(QString const& fileName) const
{
  for (auto const& series : _photoSeries)
   for (auto const& photo : *series)
    if (photo->fileName() == fileName)
     return photo;

  return nullptr;
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

void Set::removeSeries(QUuid const seriesUuid)
{
  auto& series = utils::asserted::fromPtr(findSeriesImpl(seriesUuid));
  series.removeAll();
  emit changedSeries(seriesUuid);
}

void Set::removeImagesImpl(std::vector<pcontainer::ItemId> const& itemIds)
{
  if (itemIds.empty())
    return;

  auto& series = utils::asserted::fromPtr(findSeriesImpl(itemIds.front().seriesUuid));
  series.remove(itemIds);

  emit changedSeries(series.uuid());
}

}} // namespace phobos::pcontainer
