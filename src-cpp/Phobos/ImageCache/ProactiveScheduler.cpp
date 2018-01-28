#include "ImageCache/ProactiveScheduler.h"
#include "ImageCache/Transaction.h"
#include "PhotoContainers/Set.h"
#include "PhotoContainers/Series.h"
#include "Utils/Asserted.h"
#include "Utils/Circulator.h"
#include <limits>
#include <cassert>

namespace phobos { namespace icache {

ProactiveScheduler::ProactiveScheduler(pcontainer::Set const& photoSet) :
  photoSet(photoSet), currentGeneration(0)
{}

LoadingJobVec ProactiveScheduler::operator()(Transaction && transaction)
{
  if (!transaction.loadingEnabled())
    return {};

  if (transaction.isThumbnail() || !transaction.isProactive())
    return scheduleOrganic(transaction);
  else
    return scheduleProactive(transaction);

  return utils::asserted::always;
}

namespace {
constexpr Generation const maxGeneration = std::numeric_limits<Generation>::max();

struct GenerationCounter
{
  GenerationCounter(Generation &generation, bool active) :
    trackingGeneration(generation), base(generation), active(active)
  {}

  Generation operator()(long const increment)
  {
    if (!active)
      return maxGeneration;

    Generation const result = base + increment;
    if (result > trackingGeneration)
      trackingGeneration = result;

    return result;
  }

private:
  Generation & trackingGeneration;
  Generation const base;
  bool const active;
};

template<typename It>
It advanceNotEmpty(It it, It const& end)
{
  ++it;
  while ((*it)->empty() && it != end)
    ++it;
  return it;
}

template<typename It>
It regressNotEmpty(It it, It const& end)
{
  --it;
  while ((*it)->empty() && it != end)
    --it;
  return it;
}
} // unnamed namespace

LoadingJobVec ProactiveScheduler::scheduleProactive(Transaction const& transaction)
{
  assert(transaction.isProactive());
  assert(!transaction.isThumbnail());

  auto const& mainItemId = transaction.getItemId();

  LoadingJobVec jobs;
  GenerationCounter cnt(currentGeneration, !transaction.isPersistent());

  if (lastProactiveSeries == mainItemId.seriesUuid)
  {
    jobs.emplace_back(
          LoadingJob{mainItemId, false, cnt(0), transaction.getCallback()});
    return jobs;
  }

  lastProactiveSeries = mainItemId.seriesUuid;

  jobs.emplace_back(
        LoadingJob{mainItemId, false, cnt(5), transaction.getCallback()});

  // main item for transaction
  auto const baseIt = utils::makeCirculator(photoSet.begin(), photoSet.end(),
                                            std::find_if(photoSet.begin(), photoSet.end(),
                                                         [&mainItemId](auto const& series){
                                                           return series->uuid() == mainItemId.seriesUuid;
                                                         }));

  // series in which main item is located
  for (auto const& item : **baseIt)
    if (item->id() != mainItemId)
      jobs.emplace_back(LoadingJob{item->id(), false, cnt(4), nullptr});

  // next series after main series
  auto nextIt = advanceNotEmpty(baseIt, baseIt);
  if (nextIt == baseIt)
    return jobs;

  for (auto const& item : **nextIt)
    jobs.emplace_back(LoadingJob{item->id(), false, cnt(3), nullptr});

  // prev series before main series, cannot rollback next series
  auto prevIt = regressNotEmpty(baseIt, nextIt);
  if (prevIt == nextIt)
    return jobs;

  for (auto const& item : **prevIt)
    jobs.emplace_back(LoadingJob{item->id(), false, cnt(2), nullptr});

  // second after next series, cannot rollback to prev series
  nextIt = advanceNotEmpty(nextIt, prevIt);
  if (nextIt == prevIt)
    return jobs;

  for (auto const& item : **nextIt)
    jobs.emplace_back(LoadingJob{item->id(), false, cnt(1), nullptr});

  return jobs;
}

LoadingJobVec ProactiveScheduler::scheduleOrganic(Transaction const& transaction)
{
  LoadingJobVec jobs;
  GenerationCounter cnt(currentGeneration, !transaction.isPersistent());

  jobs.emplace_back(
        LoadingJob{transaction.getItemId(), transaction.isThumbnail(), cnt(1), transaction.getCallback()});

  lastProactiveSeries = QUuid();

  return jobs;
}

// TODO: use generations to start as priorities. Handle persistent flag from transaction.
// TODO: Don't start loading proactively when there is no more cache space left for given generation

}} // namespace phobos::icache
