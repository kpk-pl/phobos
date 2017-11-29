#ifndef IMAGECACHE_PROACTIVESCHEDULER_H
#define IMAGECACHE_PROACTIVESCHEDULER_H

#include "ImageCache/TransactionFwd.h"
#include "ImageCache/LoadingJob.h"
#include "ImageCache/GenerationTracker.h"

namespace phobos { namespace icache {

class ProactiveScheduler
{
public:
  explicit ProactiveScheduler() = default;

  LoadingJobVec operator()(Transaction && parent) const;

  GenerationTracker const& generation() const;

private:
  GenerationTracker generationTracker;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_PROACTIVESCHEDULER_H
