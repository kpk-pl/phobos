#include "ImageCache/LimitedMap.h"
#include "ConfigExtension.h"
#include <easylogging++.h>
#include <cassert>

namespace phobos { namespace icache {

bool LimitedMap::has(KeyType const& key) const
{
  return map.find(key) != map.end();
}

auto LimitedMap::find(KeyType const& key) const -> ValueType
{
  auto const it = map.find(key);
  return it == map.end() ? QImage() : it->second;
}

namespace {
  double megabytes(std::size_t const bytes)
  {
    return static_cast<double>(bytes)/1024.0/1024.0;
  }
  double megabytes(QImage const& image)
  {
    return static_cast<double>(image.byteCount())/1024.0/1024.0;
  }
} // unnamed namespace

void LimitedMap::replace(KeyType const& key, ValueType const& value, Generation const& generation)
{
  auto const maxAllowedSize = config::bytes("imageCache.fullMaxBytes", 0u);
  auto const leftSize = maxAllowedSize - contentList.sizeFrom(generation + 1);
  LOG(DEBUG) << "[Cache] Left " << megabytes(leftSize) << "MB for generation " << generation
             << " (max " << megabytes(maxAllowedSize) << "MB)";

  if (static_cast<std::size_t>(value.byteCount()) > leftSize)
  {
    LOG(DEBUG) << "[Cache] Skipping full image " << key
               << " (" << megabytes(value) << "MB) due to unavailable space";
    return;
  }

  auto const it = map.find(key);

  if (it == map.end())
    insertNew(key, value, generation);
  else
    overrideExisting(it, value, generation);

  release(maxAllowedSize);
}

void LimitedMap::touch(KeyType const& key, Generation const& generation)
{
  contentList.touch(key, generation);
}

void LimitedMap::erase(KeyType const& key)
{
  auto const it = map.find(key);
  if (it != map.end())
    erase(it);
}

void LimitedMap::insertNew(KeyType const& key, ValueType const& value, Generation const& generation)
{
  contentList.insert(key, generation, value.byteCount());
  map.emplace(key, value);

  LOG(DEBUG) << "[Cache] Saved new full image " << key << " (" << megabytes(value) << "MB) at generation " << generation;
}

void LimitedMap::overrideExisting(IteratorType const& iterator, ValueType const& value, Generation const& generation)
{
  auto const& key = iterator->first;

  contentList.remove(key);
  contentList.insert(key, generation, value.byteCount());
  iterator->second = value;

  LOG(DEBUG) << "[Cache] Replaced full image " << key << " (" << megabytes(value) << "MB)";
}

void LimitedMap::erase(UnderlyingType::iterator const it)
{
  assert(it != map.end());
  LOG(DEBUG) << "[Cache] Removed full image " << it->first << " (" << megabytes(it->second) << "MB)";

  contentList.remove(it->first);
  map.erase(it);
}

void LimitedMap::release(std::size_t const maxAllowedSize)
{
  LOG(DEBUG) << "[Cache] Total full size: " << megabytes(contentList.size()) << "MB";

  while (!contentList.empty() && contentList.size() > maxAllowedSize)
    erase(map.find(contentList.pop_front()));
}

}} // namespace phobos::icache
