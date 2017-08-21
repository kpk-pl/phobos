#ifndef IMPORTWIZARD_TYPES_H
#define IMPORTWIZARD_TYPES_H

#include <string>
#include <QList>
#include <QString>
#include <boost/optional.hpp>

namespace phobos { namespace importwiz {

struct Photo
{
    QString fileName;
    boost::optional<unsigned> lastModTime;

    bool operator==(Photo const& other) const
    {
        return fileName == other.fileName && lastModTime == other.lastModTime;
    }
};

typedef QVector<Photo> PhotoSeries;
typedef QVector<PhotoSeries> PhotoSeriesVec;

}} // namespace phobos::importwiz

Q_DECLARE_METATYPE(phobos::importwiz::PhotoSeriesVec)

#endif // IMPORTWIZARD_TYPES_H
