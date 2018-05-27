#ifndef IMAGECACHE_PROACTIVESCHEDULER_H
#define IMAGECACHE_PROACTIVESCHEDULER_H

#include "ImageCache/TransactionFwd.h"
#include "PhotoContainers/Fwd.h"
#include <QUuid>

namespace phobos { namespace icache {

class ProactiveScheduler
{
public:
  explicit ProactiveScheduler(pcontainer::Set const& photoSet);

  ConstTransactionPtrVec operator()(ConstTransactionPtr transaction);

private:
  void scheduleProactive(ConstTransactionPtrVec &schedule, ConstTransactionPtr const& transaction);

  QUuid lastProactiveSeries;
  pcontainer::Set const& photoSet;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_PROACTIVESCHEDULER_H
