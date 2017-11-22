#ifndef IMAGECACHE_CONTENTLIST_H
#define IMAGECACHE_CONTENTLIST_H

#include "ImageCache/Types.h"
#include <QString>
#include <list>
#include <map>
#include <utility>

namespace phobos { namespace icache {

class ContentList
{
public:
  using Key = QString;

  explicit ContentList() = default;

  std::size_t size() const;
  std::size_t sizeBelow(Generation const generation) const;
  std::size_t sizeFrom(Generation const generation) const;

  bool empty() const;

  std::size_t insert(Key const& item, Generation const generation, std::size_t const size);
  std::size_t remove(Key const& item);
  void touch(Key const& item, Generation const generation);

  Key pop_front();
  bool exists(Key const& item);

private:
  struct Node
  {
    Key key;
    std::size_t size;
  };
  using NodeList = std::list<Node>;

  struct Bucket
  {
    void emplace_back(Key const& key, std::size_t const size);
    std::size_t erase(NodeList::iterator const& it);

    NodeList content;
    std::size_t wholeSize = 0;
  };
  using BucketsMap = std::map<Generation, Bucket>;

  std::pair<BucketsMap::iterator, NodeList::iterator> find(Key const& key);

  std::size_t remove(BucketsMap::iterator const& bucketIt, NodeList::iterator const& nodeIt);

  std::map<Generation, Bucket> buckets;
  std::size_t wholeSize = 0;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_CONTENTLIST_H
