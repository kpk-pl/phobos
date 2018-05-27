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

void LimitedMap::replace(KeyType const& key, ValueType const& value, Priority const& priority)
{
  auto const maxAllowedSize = config::bytes("imageCache.fullMaxBytes", 0u);
  auto const leftSize = maxAllowedSize - (contentList.size() - contentList.sizeBelow(priority));
  LOG(DEBUG) << "[Cache] Left " << megabytes(leftSize) << "MB for generation " << priority.toString()
             << " (max " << megabytes(maxAllowedSize) << "MB)";

  if (static_cast<std::size_t>(value.byteCount()) > leftSize)
  {
    LOG(DEBUG) << "[Cache] Skipping full image " << key
               << " (" << megabytes(value) << "MB) due to unavailable space";
    return;
  }

  auto const it = map.find(key);

  if (it == map.end())
    insertNew(key, value, priority);
  else
    overrideExisting(it, value, priority);

  release(maxAllowedSize);
}

void LimitedMap::touch(KeyType const& key, Priority const& priority)
{
  contentList.touch(key, priority);
}

void LimitedMap::erase(KeyType const& key)
{
  auto const it = map.find(key);
  if (it != map.end())
  {
    contentList.remove(it->first);
    map.erase(it);
    LOG(DEBUG) << "[Cache] Removed full image " << it->first << " (" << megabytes(it->second) << "MB)";
  }
}

void LimitedMap::insertNew(KeyType const& key, ValueType const& value, Priority const& priority)
{
  assert(contentList.insert(key, priority, value.byteCount()) > 0);
  map.emplace(key, value);

  LOG(DEBUG) << "[Cache] Saved new full image " << key << " (" << megabytes(value) << "MB) at generation " << priority.toString();
}

void LimitedMap::overrideExisting(IteratorType const& iterator, ValueType const& value, Priority const& priority)
{
  auto const& key = iterator->first;

  assert(contentList.remove(key) > 0);
  assert(contentList.insert(key, priority, value.byteCount()) > 0);
  iterator->second = value;

  LOG(DEBUG) << "[Cache] Replaced full image " << key << " (" << megabytes(value) << "MB)";
}

void LimitedMap::erase(UnderlyingType::iterator const it)
{
  assert(it != map.end());
  LOG(DEBUG) << "[Cache] Removed full image " << it->first << " (" << megabytes(it->second) << "MB)";

  assert(contentList.remove(it->first) > 0);
  map.erase(it);
}

void LimitedMap::release(std::size_t const maxAllowedSize)
{
  LOG(DEBUG) << "[Cache] Total full size: " << megabytes(contentList.size()) << "MB";

  while (!contentList.empty() && contentList.size() > maxAllowedSize)
  {
    KeyType const removed = contentList.pop_front();

    auto const it = map.find(removed);
    assert(it != map.end());
    map.erase(it);

    LOG(DEBUG) << "[Cache] Removed full image " << it->first << " (" << megabytes(it->second) << "MB)";
  }
}

}} // namespace phobos::icache
