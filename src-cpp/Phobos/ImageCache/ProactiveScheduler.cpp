#include "ImageCache/ProactiveScheduler.h"
#include "ImageCache/Transaction.h"
#include "PhotoContainers/Set.h"
#include "PhotoContainers/Series.h"
#include "Utils/Asserted.h"
#include "Utils/Circulator.h"
#include <cassert>
#include <limits>

namespace phobos { namespace icache {

ProactiveScheduler::ProactiveScheduler(pcontainer::Set const& photoSet) :
  photoSet(photoSet), currentGeneration(0)
{}

LoadingJobVec ProactiveScheduler::operator()(ConstTransactionPtr transaction)
{
  if (transaction->loadingMode == LoadingMode::Cached)
    return {};

  Schedule schedule;

  if (transaction->imageSize == ImageSize::Full && transaction->predictionMode == PredictionMode::Proactive)
  {
    schedule.proactive = scheduleProactive(transaction);
    if (schedule.proactive.empty())
      schedule.isSameProactive = true;
    lastProactiveSeries = transaction->itemId.seriesUuid;
  }
  else
  {
    lastProactiveSeries = QUuid();
  }

  schedule.organic.swap(transaction);

  return makeJobs(std::move(schedule)); // needs to dissapear
}

LoadingJobVec ProactiveScheduler::makeJobs(Schedule && schedule)
{
  LoadingJobVec result;

  // TODO: This persistecy if needs to be handles further along in Thread Pool when adding tasks to queues
  auto const gen = [&](ConstTransactionPtr && trans, int increment) -> LoadingJob {
    if (trans->persistency == Persistency::No)
      return {std::move(trans), currentGeneration + increment};
    return {std::move(trans), std::numeric_limits<Generation>::max()};
  };

  if (schedule.organic)
    result.push_back(gen(std::move(schedule.organic), schedule.proactive.empty() ? 1 : 5));

  Generation proactiveGen = currentGeneration + 5;
  for (auto& group : schedule.proactive)
  {
    --proactiveGen;
    for (auto && tran : group)
      result.push_back({std::move(tran), proactiveGen});
  }

  if (!schedule.proactive.empty())
    currentGeneration += 5;
  else if (!schedule.isSameProactive && schedule.organic)
    currentGeneration += 1;
  // else += 0

  return result;
}

namespace {
template<typename It>
It advanceNotEmpty(It it, It const& end, int increment)
{
  do
    it += increment;
  while ((*it)->empty() && it != end);

  return it;
}
} // unnamed namespace

ProactiveScheduler::Schedule::ProactiveSchedule ProactiveScheduler::scheduleProactive(ConstTransactionPtr const& transaction) const
{
  Schedule::ProactiveSchedule schedule;

  auto const& mainItemId = transaction->itemId;

  if (lastProactiveSeries == mainItemId.seriesUuid)
    return schedule;

  // main item for transaction
  auto const baseIt = utils::makeCirculator(photoSet.begin(), photoSet.end(),
                                            std::find_if(photoSet.begin(), photoSet.end(),
                                                         [&mainItemId](auto const& series){
                                                           return series->uuid() == mainItemId.seriesUuid;
                                                         }));

  // series in which main item is located
  schedule.push_back({});
  for (auto const& item : **baseIt)
    if (item->id() != mainItemId)
      schedule.back().push_back(transaction->cloneFor(item->id()));

  auto const nextGroup = [&](auto const& items){
    schedule.push_back({});
    for (auto const& item : items)
      schedule.back().push_back(transaction->cloneFor(item->id()));
  };

  // next series after main series
  auto nextIt = advanceNotEmpty(baseIt, baseIt, 1);
  if (nextIt == baseIt)
    return schedule;
  nextGroup(**nextIt);

  // prev series before main series, cannot rollback next series
  auto prevIt = advanceNotEmpty(baseIt, nextIt, -1);
  if (prevIt == nextIt)
    return schedule;
  nextGroup(**prevIt);

  // second after next series, cannot rollback to prev series
  nextIt = advanceNotEmpty(nextIt, prevIt, 1);
  if (nextIt == prevIt)
    return schedule;
  nextGroup(**nextIt);

  return schedule;
}

// TODO: use generations to start as priorities. Handle persistent flag from transaction.
// TODO: Don't start loading proactively when there is no more cache space left for given generation

}} // namespace phobos::icache
