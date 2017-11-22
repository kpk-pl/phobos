#include "ImageCache/ContentList.h"
#include <algorithm>
#include <cassert>

namespace phobos { namespace icache {
std::size_t ContentList::size() const
{
  return wholeSize;
}

std::size_t ContentList::sizeBelow(Generation const generation) const
{
  std::size_t result = 0;

  for (auto it = buckets.begin(); it != buckets.end() && it->first < generation; ++it)
    result += it->second.wholeSize;

  return result;
}

std::size_t ContentList::sizeFrom(Generation const generation) const
{
  return wholeSize - sizeBelow(generation);
}

bool ContentList::empty() const
{
  return buckets.empty();
}

std::size_t ContentList::insert(Key const& item, Generation const generation, std::size_t const size)
{
  if (exists(item))
    return 0;

  buckets[generation].emplace_back(item, size);
  wholeSize += size;
  return size;
}

std::size_t ContentList::remove(Key const& item)
{
  auto found = find(item);

  if (found.first == buckets.end())
    return 0;

  return remove(found.first, found.second);
}

std::size_t ContentList::remove(BucketsMap::iterator const& bucketIt, NodeList::iterator const& nodeIt)
{
  assert(bucketIt != buckets.end());
  auto & bucket = bucketIt->second;

  assert(nodeIt != bucket.content.end());
  auto const itemSize = nodeIt->size;

  bucket.erase(nodeIt);

  if (bucket.content.empty())
    buckets.erase(bucketIt);

  wholeSize -= itemSize;

  return itemSize;

}

void ContentList::touch(Key const& item, Generation const generation)
{
  std::size_t const removedSize = remove(item);
  if (removedSize)
    insert(item, generation, removedSize);
}

auto ContentList::pop_front() -> Key
{
  assert(!buckets.empty());

  auto const itemIt = buckets.begin()->second.content.begin();
  Key const item = itemIt->key;

  remove(buckets.begin(), itemIt);

  return item;
}

bool ContentList::exists(Key const& item)
{
  return find(item).first != buckets.end();
}

auto ContentList::find(Key const& key) ->
  std::pair<BucketsMap::iterator, NodeList::iterator>
{
  for (auto buckIt = buckets.begin(); buckIt != buckets.end(); ++buckIt)
  {
    auto& bucket = buckIt->second;
    auto nodeIt = std::find_if(bucket.content.begin(), bucket.content.end(),
                               [&key](Node const& node){ return node.key == key; });

    if (nodeIt != bucket.content.end())
      return std::make_pair(buckIt, nodeIt);
  }

  return std::make_pair(buckets.end(), NodeList::iterator{});
}

void ContentList::Bucket::emplace_back(Key const& key, std::size_t const itemSize)
{
  content.emplace_back(Node{key, itemSize});
  wholeSize += itemSize;
}

std::size_t ContentList::Bucket::erase(NodeList::iterator const& it)
{
  assert(it != content.end());

  auto const itemValue = it->size;

  content.erase(it);
  wholeSize -= itemValue;

  return itemValue;
}

}} // namespace phobos::icache
