#ifndef PHOTO_CONTAINERS_SET_H
#define PHOTO_CONTAINERS_SET_H

#include <vector>
#include <QObject>
#include <QUuid>
#include "PhotoContainers/Series.h"
#include "ImportWizard/Types.h"

namespace phobos { namespace pcontainer {

class Set : public QObject
{
    Q_OBJECT
public:
    explicit Set() = default;

    void addSeries(std::vector<importwiz::PhotoSeries> const& newPhotoSeries);
    SeriesPtr const& findSeries(QUuid const& seriesUuid,
                                int offset = 0) const;

    std::size_t size() const { return _photoSeries.size(); }
    bool empty() const { return _photoSeries.empty(); }
    SeriesPtr const& front() const;

    SeriesPtrVec::const_iterator begin() const { return _photoSeries.begin(); }
    SeriesPtrVec::const_iterator end() const { return _photoSeries.end(); }

signals:
    void newSeries(SeriesPtr);

private:
    SeriesPtrVec _photoSeries;
};

}} // namespace phobos::pcontainer

#endif // PHOTO_CONTAINERS_SET_H
