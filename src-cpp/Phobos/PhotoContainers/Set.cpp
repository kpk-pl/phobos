#include "PhotoContainers/Set.h"

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

SeriesPtr const& Set::findSeries(QUuid const& seriesUuid,
                                 int offset) const
{
    for (std::size_t i = 0; i<_photoSeries.size(); ++i)
        if (_photoSeries[i]->uuid() == seriesUuid)
            return _photoSeries[(int(i)+offset) % _photoSeries.size()];

    assert(false);
    return _photoSeries.front(); // dummy
}

}} // namespace phobos::pcontainer
