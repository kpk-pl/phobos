#ifndef IMAGECACHE_GENERATIONTRACKER_H
#define IMAGECACHE_GENERATIONTRACKER_H

#include "ImageCache/Types.h"

namespace phobos { namespace icache {

class GenerationTracker
{
public:
  explicit GenerationTracker() = default;

  Generation current() const { return 0; }
};

}} // namespace phobos::icache

#endif // IMAGECACHE_GENERATIONTRACKER_H
