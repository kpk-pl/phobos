#ifndef IMAGECACHE_TYPES_H
#define IMAGECACHE_TYPES_H

namespace phobos { namespace icache {

enum class ImageSize
{
  Thumbnail,
  Full
};

enum class Persistency : bool
{
  No = false,
  Yes = true
};

enum class PredictionMode
{
  None,
  Proactive
};

enum class LoadingMode
{
  Cached,
  Active
};

}} // namespace phobos::icache

#endif // IMAGECACHE_TYPES_H
