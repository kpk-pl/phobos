#ifndef PHOTO_CONTAINERS_SET_H
#define PHOTO_CONTAINERS_SET_H

#include <vector>
#include <QObject>
#include <QUuid>
#include "PhotoContainers/Series.h"

namespace phobos { namespace pcontainer {

class Set : public QObject
{
    Q_OBJECT
public:
    explicit Set() = default;

    void addPhotos(QStringList const& fileNames);
    SeriesPtr const& findSeries(QUuid const& seriesUuid,
                                int offset = 0) const;

    std::size_t size() const { return photoSeries.size(); }
    bool empty() const { return photoSeries.empty(); }
    SeriesPtr const& front() const;

    SeriesPtrVec::const_iterator begin() const { return photoSeries.begin(); }
    SeriesPtrVec::const_iterator end() const { return photoSeries.end(); }

signals:
    void newSeries(SeriesPtr);

private:
    SeriesPtrVec photoSeries;
};

}} // namespace phobos::pcontainer

#endif // PHOTO_CONTAINERS_SET_H
