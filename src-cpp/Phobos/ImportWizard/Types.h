#ifndef IMPORTWIZARD_TYPES_H
#define IMPORTWIZARD_TYPES_H

#include <QList>
#include <QString>
#include <QImage>
#include "PhotoContainers/FileInfo.h"

namespace phobos { namespace importwiz {

struct Photo
{
  QString name;
  pcontainer::FileInfo info;

  bool operator==(Photo const& other) const
  {
    return name == other.name && info == other.info;
  }

  struct TimeNameComp
  {
    bool operator()(Photo const& lhs, Photo const& rhs) const;
  };
};

struct PhotoSeries : public QVector<Photo>
{
  bool isASeries = true;
};

using PhotoSeriesVec = QVector<PhotoSeries>;

}} // namespace phobos::importwiz

Q_DECLARE_METATYPE(phobos::importwiz::PhotoSeriesVec)

#endif // IMPORTWIZARD_TYPES_H
