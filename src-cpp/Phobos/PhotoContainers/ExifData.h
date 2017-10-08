#ifndef PHOTOCONTAINERS_EXIFDATA_H
#define PHOTOCONTAINERS_EXIFDATA_H

#include <ImportWizard/Types.h>
#include <QSize>
#include <QDateTime>

namespace phobos { namespace pcontainer {

struct ExifData
{
public:
  explicit ExifData(importwiz::Photo const& photoDesc);

  QSize size;
  QDateTime timestamp;
};

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_EXIFDATA_H
