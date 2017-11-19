#ifndef PHOTOCONTAINERS_EXIFDATA_H
#define PHOTOCONTAINERS_EXIFDATA_H

#include <QString>
#include <QSize>
#include <QDateTime>
#include <QImage>
#include <boost/optional.hpp>
#include <utility>

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
    boost::optional<QString> make;
    boost::optional<QString> model;
    bool operator==(CameraInfo const& other) const;
    bool operator!=(CameraInfo const& other) const;
  };

  struct ShotInfo
  {
    using Rational = std::pair<long, long>;
    boost::optional<short> orientation;
    boost::optional<Rational> exposureTime; // ratio num/denom
    boost::optional<double> exposureBias; // in EV
    boost::optional<Rational> fnumber; // F/stop
    boost::optional<unsigned> ISOSpeed;
    boost::optional<double> focalLen; // in mm
    boost::optional<bool> flash;
  };

  CameraInfo camera;
  ShotInfo shot;

  QImage thumbnail;

  bool operator==(FileInfo const& other) const;
};

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_EXIFDATA_H
