#include "Views/View.h"

namespace phobos { namespace view {

View::View(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
  seriesSet(seriesSet), imageCache(imageCache)
{}

}} // namespace phobos::view
