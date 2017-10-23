#ifndef PHOTOCONTAINERS_EXIFDATA_H
#define PHOTOCONTAINERS_EXIFDATA_H

#include <QString>
#include <QSize>
#include <QDateTime>
#include <boost/optional.hpp>

namespace phobos { namespace pcontainer {

struct FileInfo
{
public:
  FileInfo() = default;
  explicit FileInfo(QString const& fileName);

  QSize size;
  unsigned timestamp;

  struct CameraInfo
  {
    QString make, model;
    bool operator==(CameraInfo const& other) const;
  };

  struct ShotInfo
  {
    double exposureTime; // in sec
    double exposureBias; // in EV
    double fnumber; // F/stop
    unsigned ISOSpeed;
    double focalLen; // in mm
    bool flash;
  };

  boost::optional<CameraInfo> camera;
  boost::optional<ShotInfo> shot;

  bool operator==(FileInfo const& other) const;
};

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_EXIFDATA_H
