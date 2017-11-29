#include "ImageCache/ProactiveScheduler.h"
#include "ImageCache/Transaction.h"

namespace phobos { namespace icache {

LoadingJobVec ProactiveScheduler::operator()(Transaction && transaction) const
{
  LoadingJobVec result;

  result.emplace_back(LoadingJob{transaction.getItemId(),
                                 transaction.isThumbnail(),
                                 generation().current(),
                                 transaction.getCallback()});

  return result;
}

// TODO: use generations to start as priorities. Handle persistent flag from transaction.
// Don't start loading proactively when there is no more cache space left for given generation

GenerationTracker const& ProactiveScheduler::generation() const
{
  return generationTracker;
}

}} // namespace phobos::icache
