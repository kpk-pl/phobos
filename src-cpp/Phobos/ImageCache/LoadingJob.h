#ifndef IMAGECACHE_LOADINGJOB_H
#define IMAGECACHE_LOADINGJOB_H

#include "PhotoContainers/ItemId.h"
#include "ImageCache/TransactionFwd.h"

namespace phobos { namespace icache {

struct LoadingJob
{
  pcontainer::ItemId itemId;
  bool onlyThumbnail;
  OptTransactionCallback callback;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_LOADINGJOB_H
