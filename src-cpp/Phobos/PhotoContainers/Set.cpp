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
  SeriesPtr const& series = findSeries(itemId.seriesUuid);
  series->remove(itemId.fileName);
  // possible that this series remains empty forever

  emit changedSeries(series->uuid());
}

}} // namespace phobos::pcontainer
