#ifndef PHOBOS_PHOTOCONTAINERS_FWD_H_
#define PHOBOS_PHOTOCONTAINERS_FWD_H_

#include <memory>
#include <vector>

namespace phobos { namespace pcontainer {

class Item;
using ItemPtr = std::shared_ptr<Item>;
using ItemPtrVec = std::vector<ItemPtr>;

class Series;
using SeriesPtr = std::shared_ptr<Series>;
using SeriesPtrVec = std::vector<SeriesPtr>;

class Set;

}} // namespace phobos::pcontainer

#endif // PHOBOS_PHOTOCONTAINERS_FWD_H_
