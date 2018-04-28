#ifndef IMAGECACHE_LOADINGJOB_H
#define IMAGECACHE_LOADINGJOB_H

#include "PhotoContainers/ItemId.h"
#include "ImageCache/TransactionFwd.h"
#include "ImageCache/Types.h"
#include <vector>

namespace phobos { namespace icache {

struct LoadingJob
{
  ConstTransactionPtr transaction;
  Generation generation; // the higher the generation the higher the load priority is
};

using LoadingJobVec = std::vector<LoadingJob>;

}} // namespace phobos::icache

#endif // IMAGECACHE_LOADINGJOB_H
