#ifndef PROCESSWIZARD_SERIESCOUNTS_H
#define PROCESSWIZARD_SERIESCOUNTS_H

#include "PhotoContainers/ItemState.h"
#include "PhotoContainers/Fwd.h"
#include <map>

namespace phobos { namespace processwiz {

struct SeriesCounts
{
  struct TypeCounts
  {
    std::size_t photos = 0;
    std::size_t series = 0;
  };

  TypeCounts all;
  std::map<pcontainer::ItemState, TypeCounts> types;

  std::size_t seriesCompletelyDiscarded = 0;
  std::size_t seriesCompletelyUnknown = 0;
};

SeriesCounts countPhotos(pcontainer::Set const& seriesSet);

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_SERIESCOUNTS_H
