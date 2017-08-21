#ifndef PHOTOCONTAINERS_EXIFDATA_H
#define PHOTOCONTAINERS_EXIFDATA_H

#include <QString>
#include <QDateTime>
#include <boost/optional.hpp>

namespace phobos { namespace pcontainer {

struct ExifData
{
public:
  explicit ExifData(QString const& fileName);

  boost::optional<std::size_t> width;
  boost::optional<std::size_t> height;
  QDateTime dateTime;
};

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_EXIFDATA_H
