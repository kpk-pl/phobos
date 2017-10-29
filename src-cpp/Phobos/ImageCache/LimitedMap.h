#ifndef PHOBOS_IMAGECACHE_LIMITED_MAP_H_
#define PHOBOS_IMAGECACHE_LIMITED_MAP_H_

#include "ImageCache/ContentList.h"
#include <map>
#include <list>
#include <QString>
#include <QImage>

namespace phobos { namespace icache {

class LimitedMap
{
public:
  using KeyType = QString;
  using ValueType = QImage;
  using UnderlyingType = std::map<KeyType, ValueType>;

  LimitedMap() = default;

  ValueType find(KeyType const& key) const;
  void replace(KeyType const& key, ValueType const& value);
  void erase(KeyType const& key);

private:
  using IteratorType = typename UnderlyingType::iterator;

  UnderlyingType map;
  ContentList contentList;

  void insertNew(KeyType const& key, ValueType const& value);
  void overrideExisting(IteratorType const& iterator, ValueType const& value);
  void erase(UnderlyingType::iterator const it);
  void release(std::size_t const maxAllowedSize);
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_LIMITED_MAP_H_
