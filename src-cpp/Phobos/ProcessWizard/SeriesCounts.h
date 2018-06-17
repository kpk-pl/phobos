#ifndef PROCESSWIZARD_SERIESCOUNTS_H
#define PROCESSWIZARD_SERIESCOUNTS_H

#include "PhotoContainers/Fwd.h"

namespace phobos { namespace processwiz {

struct SeriesCounts
{
  struct TypeCounts
  {
    std::size_t photos = 0;
    std::size_t series = 0;
    void add(std::size_t const photosInSeries);
  };

  TypeCounts allFree;
  TypeCounts allSeries;
  TypeCounts selectedFree;
  TypeCounts selectedSeries;
  TypeCounts ignoredFree;
  TypeCounts ignoredSeries;
};

SeriesCounts countPhotos(pcontainer::Set const& seriesSet);

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_SERIESCOUNTS_H
