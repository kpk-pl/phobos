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

  if (transaction->imageSize == ImageSize::Thumbnail)
    return scheduleOrganic(std::move(transaction));

  if (transaction->predictionMode == PredictionMode::None)
    return scheduleOrganic(std::move(transaction));

  return scheduleProactive(std::move(transaction));
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

LoadingJobVec ProactiveScheduler::scheduleProactive(ConstTransactionPtr transaction)
{
  assert(transaction->predictionMode == PredictionMode::Proactive);
  assert(transaction->imageSize == ImageSize::Full);

  auto const& mainItemId = transaction->itemId;

  LoadingJobVec jobs;
  GenerationCounter cnt(currentGeneration, transaction->persistency == Persistency::No);

  if (lastProactiveSeries == mainItemId.seriesUuid)
  {
    jobs.push_back({std::move(transaction), cnt(0)});
    return jobs;
  }

  lastProactiveSeries = mainItemId.seriesUuid;

  jobs.push_back({std::move(transaction), cnt(5)});

  // main item for transaction
  auto const baseIt = utils::makeCirculator(photoSet.begin(), photoSet.end(),
                                            std::find_if(photoSet.begin(), photoSet.end(),
                                                         [&mainItemId](auto const& series){
                                                           return series->uuid() == mainItemId.seriesUuid;
                                                         }));

  auto const parentTransaction = jobs.front().transaction;

  // series in which main item is located
  for (auto const& item : **baseIt)
    if (item->id() != mainItemId)
      jobs.push_back({parentTransaction->cloneFor(item->id()), cnt(4)});

  // next series after main series
  auto nextIt = advanceNotEmpty(baseIt, baseIt);
  if (nextIt == baseIt)
    return jobs;

  for (auto const& item : **nextIt)
    jobs.push_back({parentTransaction->cloneFor(item->id()), cnt(3)});

  // prev series before main series, cannot rollback next series
  auto prevIt = regressNotEmpty(baseIt, nextIt);
  if (prevIt == nextIt)
    return jobs;

  for (auto const& item : **prevIt)
    jobs.push_back({parentTransaction->cloneFor(item->id()), cnt(2)});

  // second after next series, cannot rollback to prev series
  nextIt = advanceNotEmpty(nextIt, prevIt);
  if (nextIt == prevIt)
    return jobs;

  for (auto const& item : **nextIt)
    jobs.push_back({parentTransaction->cloneFor(item->id()), cnt(1)});

  return jobs;
}

LoadingJobVec ProactiveScheduler::scheduleOrganic(ConstTransactionPtr transaction)
{
  lastProactiveSeries = QUuid();
  GenerationCounter cnt(currentGeneration, transaction->persistency == Persistency::No);

  LoadingJobVec jobs;
  jobs.push_back({std::move(transaction), cnt(1)});
  return jobs;
}

// TODO: use generations to start as priorities. Handle persistent flag from transaction.
// TODO: Don't start loading proactively when there is no more cache space left for given generation

}} // namespace phobos::icache
