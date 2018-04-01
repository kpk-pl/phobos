#include "ProcessWizard/SeriesCounts.h"
#include "PhotoContainers/Set.h"
#include <easylogging++.h>
#include <algorithm>

namespace phobos { namespace processwiz {

void SeriesCounts::TypeCounts::add(std::size_t const photosInSeries)
{
  if (photosInSeries == 0)
    return;

  photos += photosInSeries;
  series += 1;
}

namespace {
using ItemState = pcontainer::ItemState;
static ItemState const allStates[] = { ItemState::SELECTED, ItemState::UNKNOWN };

struct Counter
{
  Counter(SeriesCounts &seriesCounts) :
    seriesCounts(seriesCounts)
  {}

  void operator()(pcontainer::SeriesPtr const& series) const
  {
    auto const match = [](pcontainer::ItemState state){ return [state](auto const& photo){ return photo->state() == state; }; };
    std::size_t const selectedCount = std::count_if(series->begin(), series->end(), match(pcontainer::ItemState::SELECTED));
    std::size_t const unknownCount = std::count_if(series->begin(), series->end(), match(pcontainer::ItemState::UNKNOWN));

    seriesCounts.all.add(series->size());
    seriesCounts.selected.add(selectedCount);
    seriesCounts.unknown.add(unknownCount);
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

  LOG(TRACE) << "Found " << counts.all.photos << " photos in " << counts.all.series << " series in total";
  LOG(TRACE) << "Found " << counts.selected.photos << " selected photos in " << counts.selected.series << " series";
  LOG(TRACE) << "Found " << counts.unknown.photos << " unknown photos in " << counts.unknown.series << " series";

  return counts;
}

}} // namespace phobos::processwiz
