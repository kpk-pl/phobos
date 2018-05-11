#ifndef PHOTO_CONTAINERS_SERIES_H
#define PHOTO_CONTAINERS_SERIES_H

#include "PhotoContainers/Fwd.h"
#include "PhotoContainers/Item.h"
#include "PhotoContainers/ItemId.h"
#include "ImportWizard/Types.h"
#include <QUuid>
#include <QObject>
#include <QString>
#include <vector>

namespace phobos { namespace pcontainer {

class Series : public QObject
{
  Q_OBJECT

public:
  explicit Series(std::size_t const pOrd, importwiz::PhotoSeries const& series);

  std::size_t size() const { return _photoItems.size(); }
  bool empty() const { return _photoItems.empty(); }
  QUuid const& uuid() const { return _uuid; }
  std::size_t ord() const { return _ord; }

  bool const isPhotoSeries = true;

  ItemPtrVec::const_reference operator[](std::size_t i) const { return _photoItems[i]; }
  ItemPtrVec::const_reference item(std::size_t i) const { return _photoItems[i]; }
  ItemPtrVec::const_reference front() const { return _photoItems.front(); }
  ItemPtrVec::const_reference back() const { return _photoItems.back(); }
  ItemPtrVec::const_iterator begin() const { return _photoItems.begin(); }
  ItemPtrVec::const_iterator end() const { return _photoItems.end(); }

  void remove(std::vector<pcontainer::ItemId> const& itemIds);
  void removeAll();
  std::vector<ItemId> const& removedItems() const { return _removedItems; }

private:
  void addPhotoItem(importwiz::Photo const& importedPhoto);

  ItemPtrVec _photoItems;
  std::vector<ItemId> _removedItems;

  QUuid const _uuid;
  std::size_t const _ord;
};

}} // namespace phobos::pcontainer

#endif // PHOTO_CONTAINERS_SERIES_H
