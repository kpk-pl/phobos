#include "PhotoContainers/Set.h"
#include "Utils/Comparators.h"
#include "Utils/Asserted.h"

namespace phobos { namespace pcontainer {

Series const& Set::front() const
{
  return utils::asserted::fromPtr(_photoSeries.front());
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

namespace {
  std::size_t posModulo(int val, int mod)
  {
    val %= mod;
    if (val < 0)
      val += mod;
    return val;
  }
}

SeriesPtr const& Set::findSeriesImpl(QUuid const& seriesUuid, int offset) const
{
  for (std::size_t i = 0; i<_photoSeries.size(); ++i)
    if (_photoSeries[i]->uuid() == seriesUuid)
      return _photoSeries[posModulo(int(i)+offset, _photoSeries.size())];

  return utils::asserted::always;
}

Series const& Set::findSeries(QUuid const& seriesUuid, int offset) const
{
  return utils::asserted::fromPtr(findSeriesImpl(seriesUuid, offset));
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

  auto& series = utils::asserted::fromPtr(findSeriesImpl(itemIds.front().seriesUuid));
  series.remove(itemIds);

  emit changedSeries(series.uuid());
}

}} // namespace phobos::pcontainer
