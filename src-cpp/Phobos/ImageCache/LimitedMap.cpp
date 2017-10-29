#include <cassert>
#include "ImageCache/LimitedMap.h"
#include "ConfigExtension.h"
#include <easylogging++.h>

namespace phobos { namespace icache {

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

void LimitedMap::replace(KeyType const& key, ValueType const& value)
{
  auto const maxAllowedSize = config::bytes("imageCache.fullMaxBytes", 0u);
  LOG(DEBUG) << "[Cache] Full cache maximum size is " << megabytes(maxAllowedSize) << "MB";

  if (static_cast<std::size_t>(value.byteCount()) > maxAllowedSize)
  {
    LOG(DEBUG) << "[Cache] Skipping full image " << key << " due to unavailable space"
                  " (" << megabytes(value) << "MB)";
    return;
  }

  auto const it = map.find(key);

  if (it == map.end())
    insertNew(key, value);
  else
    overrideExisting(it, value);

  release(maxAllowedSize);
}

void LimitedMap::erase(KeyType const& key)
{
  auto const it = map.find(key);
  if (it != map.end())
    erase(it);
}

void LimitedMap::insertNew(KeyType const& key, ValueType const& value)
{
  contentList.insert(key, value.byteCount());
  map.emplace(key, value);

  LOG(DEBUG) << "[Cache] Saved new full image " << key << " (" << megabytes(value) << "MB)";
}

void LimitedMap::overrideExisting(IteratorType const& iterator, ValueType const& value)
{
  auto const& key = iterator->first;

  contentList.remove(key);
  contentList.insert(key, value.byteCount());
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
