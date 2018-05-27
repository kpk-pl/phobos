#ifndef IMAGECACHE_PRIORITY_H
#define IMAGECACHE_PRIORITY_H

#include <chrono>
#include <string>

namespace phobos { namespace icache {

struct Priority
{
  using Clock = std::chrono::high_resolution_clock;

  Clock::time_point timestamp;
  unsigned proactiveGeneration;

  bool operator==(Priority const& rhs) const
  {
    return timestamp == rhs.timestamp && proactiveGeneration == rhs.proactiveGeneration;
  }

  bool operator!=(Priority const& rhs) const
  {
    return !((*this) == rhs);
  }

  bool operator<(Priority const& rhs) const
  {
    if (timestamp != rhs.timestamp)
      return timestamp < rhs.timestamp;

    return proactiveGeneration > rhs.proactiveGeneration; // biggest generations first
  }

  std::string toString() const
  {
    return std::to_string(timestamp.time_since_epoch().count()) + "/" + std::to_string(proactiveGeneration);
  }
};

}} // namespace phobos::icache

#endif // IMAGECACHE_PRIORITY_H
