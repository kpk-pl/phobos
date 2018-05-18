#ifndef IMAGECACHE_PROACTIVESCHEDULER_H
#define IMAGECACHE_PROACTIVESCHEDULER_H

#include "ImageCache/TransactionFwd.h"
#include "ImageCache/LoadingJob.h"
#include "PhotoContainers/Fwd.h"

namespace phobos { namespace icache {

class ProactiveScheduler
{
public:
  struct Schedule
  {
    ConstTransactionPtr organic;

    using TransactionGroup = std::vector<ConstTransactionPtr>;
    using ProactiveSchedule = std::vector<TransactionGroup>;
    ProactiveSchedule proactive;

    bool isSameProactive = false; // TODO: to be removed
  };

  explicit ProactiveScheduler(pcontainer::Set const& photoSet);

  LoadingJobVec operator()(ConstTransactionPtr transaction);

private:
  Schedule::ProactiveSchedule scheduleProactive(ConstTransactionPtr const& transaction) const;
  LoadingJobVec makeJobs(Schedule && schedule);

  pcontainer::Set const& photoSet;
  Generation currentGeneration;

  QUuid lastProactiveSeries;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_PROACTIVESCHEDULER_H
