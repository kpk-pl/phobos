#ifndef PHOBOS_IMAGECACHE_LIMITED_MAP_H_
#define PHOBOS_IMAGECACHE_LIMITED_MAP_H_

#include <unordered_map>
#include <list>
#include <string>
#include <QImage>

namespace phobos { namespace icache {

class LimitedMap
{
public:
  using KeyType = std::string;
  using ValueType = QImage;
  using UnderlyingType = std::unordered_map<KeyType, ValueType>;

  LimitedMap() = default;

  ValueType find(KeyType const& key) const;
  void replace(KeyType const& key, ValueType const& value);

private:
  using IteratorType = typename UnderlyingType::iterator;

  UnderlyingType map;

  std::size_t contentSize = 0;
  std::list<KeyType> insertOrder;

  void insertNew(KeyType const& key, ValueType const& value);
  void overrideExisting(IteratorType const& iterator, ValueType const& value);
  void release(std::size_t const maxAllowedSize);
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_LIMITED_MAP_H_
