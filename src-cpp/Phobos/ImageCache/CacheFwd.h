#ifndef PHOBOS_IMAGECACHE_CACHEFWD_H_
#define PHOBOS_IMAGECACHE_CACHEFWD_H_

#include <QImage>

namespace phobos { namespace icache {

class Cache;

enum class ImageQuality
{
  None, Blank, ExifThumb, Thumb, Full
};

struct Result
{
  QImage image;
  ImageQuality quality;
  bool sufficient;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_CACHEFWD_H_
