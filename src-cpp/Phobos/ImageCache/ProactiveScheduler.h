#ifndef IMAGECACHE_PROACTIVESCHEDULER_H
#define IMAGECACHE_PROACTIVESCHEDULER_H

#include "ImageCache/TransactionFwd.h"
#include "ImageCache/LoadingJob.h"
#include "PhotoContainers/Fwd.h"

namespace phobos { namespace icache {

class ProactiveScheduler
{
public:
  explicit ProactiveScheduler(pcontainer::Set const& photoSet);

  LoadingJobVec operator()(Transaction && parent);

private:
  LoadingJobVec scheduleProactive(Transaction const& transaction);
  LoadingJobVec scheduleOrganic(Transaction const& transaction);

  pcontainer::Set const& photoSet;
  Generation currentGeneration;

  QUuid lastProactiveSeries;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_PROACTIVESCHEDULER_H
