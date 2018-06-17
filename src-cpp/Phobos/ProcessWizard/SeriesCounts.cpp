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
static ItemState const allStates[] = { ItemState::SELECTED, ItemState::IGNORED };

struct Counter
{
  Counter(SeriesCounts &seriesCounts) :
    seriesCounts(seriesCounts)
  {}

  void operator()(pcontainer::SeriesPtr const& series) const
  {
    auto const match = [](pcontainer::ItemState state){ return [state](auto const& photo){ return photo->state() == state; }; };
    std::size_t const selectedCount = std::count_if(series->begin(), series->end(), match(pcontainer::ItemState::SELECTED));
    std::size_t const unknownCount = std::count_if(series->begin(), series->end(), match(pcontainer::ItemState::IGNORED));

    if (series->isPhotoSeries)
    {
      seriesCounts.allSeries.add(series->size());
      seriesCounts.selectedSeries.add(selectedCount);
      seriesCounts.ignoredSeries.add(unknownCount);
    }
    else
    {
      seriesCounts.allFree.add(series->size());
      seriesCounts.selectedFree.add(selectedCount);
      seriesCounts.ignoredFree.add(unknownCount);
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

  LOG(TRACE) << "Found " << counts.allSeries.photos << " photos in " << counts.allSeries.series << " series in total";
  LOG(TRACE) << "Found " << counts.selectedSeries.photos << " selected photos in " << counts.selectedSeries.series << " series";
  LOG(TRACE) << "Found " << counts.ignoredSeries.photos << " unknown photos in " << counts.ignoredSeries.series << " series";

  LOG(TRACE) << "Found " << counts.allFree.photos << " photos in total";
  LOG(TRACE) << "Found " << counts.selectedFree.photos << " selected photos";
  LOG(TRACE) << "Found " << counts.ignoredFree.photos << " unknown photos";

  return counts;
}

}} // namespace phobos::processwiz
