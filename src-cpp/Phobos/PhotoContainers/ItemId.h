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

  bool operator==(ItemId const& rhs) const
  {
    return fileName == rhs.fileName && seriesUuid == rhs.seriesUuid;
  }
  bool operator<(ItemId const& rhs) const
  {
    return (seriesUuid == rhs.seriesUuid) ? fileName < rhs.fileName : seriesUuid < rhs.seriesUuid;
  }
};

}} // namespace phobos::pcontainer

Q_DECLARE_METATYPE(phobos::pcontainer::ItemId)

#endif // PHOTOCONTAINERS_ITEMID_H
