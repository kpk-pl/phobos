#include "ImageCache/ContentList.h"
#include <algorithm>
#include <cassert>

namespace phobos { namespace icache {

bool ContentList::Node::operator<(Node const& rhs) const
{
  if (priority != rhs.priority)
    return priority < rhs.priority;

  if (size != rhs.size)
    return size > rhs.size; // biggest first

  return key < rhs.key;
}

std::size_t ContentList::size() const
{
  return wholeSize;
}

std::size_t ContentList::sizeBelow(Priority const& priority) const
{
  auto const lb = std::lower_bound(content.begin(), content.end(), priority, [](Node const& l, Priority const& r){
    return l.priority < r;
  });

  return std::accumulate(content.begin(), lb, 0, [](std::size_t v, auto const& node){
    return v + node.size;
  });
}

bool ContentList::empty() const
{
  return content.empty();
}

std::size_t ContentList::insert(Key const& item, Priority const& priority, std::size_t const size)
{
  if (exists(item))
    return 0;

  assert(content.insert(Node{item, priority, size}).second);
  wholeSize += size;
  return size;
}

std::size_t ContentList::remove(Key const& item)
{
  auto it = find(item);

  if (it == content.end())
    return 0;

  auto const itemSize = it->size;

  content.erase(it);
  wholeSize -= itemSize;

  return itemSize;
}

void ContentList::touch(Key const& item, Priority const& generation)
{
  auto const removedSize = remove(item);
  assert(removedSize > 0);
  insert(item, generation, removedSize);
}

auto ContentList::pop_front() -> Key
{
  assert(!content.empty());

  Key const item = content.begin()->key;

  wholeSize -= content.begin()->size;
  content.erase(content.begin());

  return item;
}

bool ContentList::exists(Key const& item) const
{
  return find(item) != content.end();
}

std::set<ContentList::Node>::const_iterator ContentList::find(Key const& item) const
{
  return std::find_if(content.begin(), content.end(), [&](Node const& node){
    return node.key == item;
  });
}

}} // namespace phobos::icache
