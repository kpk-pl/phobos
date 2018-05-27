#include "ImageCache/ProactiveScheduler.h"
#include "ImageCache/Transaction.h"
#include "PhotoContainers/Set.h"
#include "PhotoContainers/Series.h"
#include "Utils/Asserted.h"
#include "Utils/Circulator.h"
#include <cassert>

namespace phobos { namespace icache {

ProactiveScheduler::ProactiveScheduler(pcontainer::Set const& photoSet) :
  photoSet(photoSet)
{}

ConstTransactionPtrVec ProactiveScheduler::operator()(ConstTransactionPtr transaction)
{
  ConstTransactionPtrVec schedule;

  if (transaction->loadingMode == LoadingMode::Cached)
    return schedule;

  schedule.push_back(nullptr);
  if (transaction->imageSize == ImageSize::Full && transaction->predictionMode == PredictionMode::Proactive)
    scheduleProactive(schedule, transaction);
  else
    lastProactiveSeries = QUuid();

  std::swap(schedule.front(), transaction);
  return schedule;
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

void ProactiveScheduler::scheduleProactive(ConstTransactionPtrVec &schedule, ConstTransactionPtr const& transaction)
{
  auto const& mainItemId = transaction->itemId;

  if (lastProactiveSeries == mainItemId.seriesUuid)
    return;

  // main item for transaction
  auto const baseIt = utils::makeCirculator(photoSet.begin(), photoSet.end(),
                                            std::find_if(photoSet.begin(), photoSet.end(),
                                                         [&mainItemId](auto const& series){
                                                           return series->uuid() == mainItemId.seriesUuid;
                                                         }));

  // series in which main item is located
  for (auto const& item : **baseIt)
    if (item->id() != mainItemId)
      schedule.push_back(transaction->cloneFor(item->id(), 1));

  auto const nextGroup = [&](auto const& items, unsigned const generation){
    for (auto const& item : items)
      schedule.push_back(transaction->cloneFor(item->id(), generation));
  };

  // next series after main series
  auto nextIt = advanceNotEmpty(baseIt, baseIt, 1);
  if (nextIt == baseIt)
    return;
  nextGroup(**nextIt, 2);

  // prev series before main series, cannot rollback next series
  auto prevIt = advanceNotEmpty(baseIt, nextIt, -1);
  if (prevIt == nextIt)
    return;
  nextGroup(**prevIt, 3);

  // second after next series, cannot rollback to prev series
  nextIt = advanceNotEmpty(nextIt, prevIt, 1);
  if (nextIt == prevIt)
    return;
  nextGroup(**nextIt, 4);

  lastProactiveSeries = mainItemId.seriesUuid;
}

}} // namespace phobos::icache
