#ifndef PHOTOCONTAINERS_ITEMID_H
#define PHOTOCONTAINERS_ITEMID_H

#include <QString>
#include <QUuid>
#include <QMetaType>

namespace phobos { namespace pcontainer {

struct ItemId
{
  QUuid seriesUuid;
  QString fileName;

  operator bool() const
  {
    return !seriesUuid.isNull() && !fileName.isEmpty();
  }

  bool operator==(ItemId const& rhs) const
  {
    return fileName == rhs.fileName && seriesUuid == rhs.seriesUuid;
  }

  bool operator!=(ItemId const& rhs) const
  {
    return fileName != rhs.fileName || seriesUuid != rhs.seriesUuid;
  }

  bool operator<(ItemId const& rhs) const
  {
    return (seriesUuid == rhs.seriesUuid) ? fileName < rhs.fileName : seriesUuid < rhs.seriesUuid;
  }

  std::string toString() const
  {
    return "[" + fileName.toStdString() + "@" + seriesUuid.toString().toStdString() + "]";
  }
};

}} // namespace phobos::pcontainer

Q_DECLARE_METATYPE(phobos::pcontainer::ItemId)

#endif // PHOTOCONTAINERS_ITEMID_H
