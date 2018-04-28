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

  LoadingJobVec operator()(ConstTransactionPtr transaction);

private:
  LoadingJobVec scheduleProactive(ConstTransactionPtr transaction);
  LoadingJobVec scheduleOrganic(ConstTransactionPtr transaction);

  pcontainer::Set const& photoSet;
  Generation currentGeneration;

  QUuid lastProactiveSeries;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_PROACTIVESCHEDULER_H
