#ifndef IMAGECACHE_CONTENTLIST_H
#define IMAGECACHE_CONTENTLIST_H

#include "ImageCache/Priority.h"
#include <QString>
#include <list>
#include <set>
#include <utility>

namespace phobos { namespace icache {

class ContentList
{
public:
  using Key = QString;

  explicit ContentList() = default;

  std::size_t size() const;
  std::size_t sizeBelow(Priority const& priority) const;

  bool empty() const;

  std::size_t insert(Key const& item, Priority const& priority, std::size_t const size);
  std::size_t remove(Key const& item);
  void touch(Key const& item, Priority const& priority);

  Key pop_front();
  bool exists(Key const& item) const;

private:
  struct Node
  {
    Key key;
    Priority priority;
    std::size_t size;

    bool operator<(Node const& rhs) const;
  };

  std::set<Node>::const_iterator find(Key const& item) const;

  std::set<Node> content;
  std::size_t wholeSize = 0;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_CONTENTLIST_H
