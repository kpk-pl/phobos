#include "ProcessWizard/SeriesCounts.h"
#include "PhotoContainers/Set.h"
#include <easylogging++.h>
#include <algorithm>

namespace phobos { namespace processwiz {

namespace {
using ItemState = pcontainer::ItemState;
static ItemState const allStates[] = { ItemState::SELECTED, ItemState::DISCARDED, ItemState::UNKNOWN };

struct Counter
{
  Counter(SeriesCounts &seriesCounts) :
    seriesCounts(seriesCounts)
  {}

  void operator()(pcontainer::SeriesPtr const& series) const
  {
    seriesCounts.all.series += 1;
    seriesCounts.all.photos += series->size();

    std::map<ItemState, std::size_t> counts;
    for (auto const& photo : *series)
      ++counts[photo->state()];

    for (auto const& group : counts)
    {
      seriesCounts.types[group.first].photos += group.second;
      ++seriesCounts.types[group.first].series;
    }

    if (counts.size() == 1)
    {
      if (counts.begin()->first == ItemState::DISCARDED)
        ++seriesCounts.seriesCompletelyDiscarded;
      else if (counts.begin()->first == ItemState::UNKNOWN)
        ++seriesCounts.seriesCompletelyUnknown;
    }
  }

private:
  SeriesCounts &seriesCounts;
};
} // unnamed namespace

SeriesCounts countPhotos(pcontainer::Set const& seriesSet)
{
  SeriesCounts counts;
  Counter const counter(counts);
  std::for_each(seriesSet.begin(), seriesSet.end(), std::ref(counter));

  LOG(INFO) << "[processing] Found " << counts.all.photos << " photos in " << counts.all.series << " series in total";

  // Accessing counts with [] creates all possible states
  for (pcontainer::ItemState const state : allStates)
    LOG(INFO) << "[processing] Found " << counts.types[state].photos << " " << state << " photos in "
              << counts.types[state].series << " series";

  LOG(INFO) << "[processing] Found " << counts.seriesCompletelyDiscarded << " completely discarded series";
  LOG(INFO) << "[processing] Found " << counts.seriesCompletelyUnknown << " completely unchecked series";

  return counts;
}

}} // namespace phobos::processwiz
