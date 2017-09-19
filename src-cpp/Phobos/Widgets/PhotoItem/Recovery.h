#ifndef WIDGETS_PHOTOITEM_RECOVERY_H
#define WIDGETS_PHOTOITEM_RECOVERY_H

#include "PhotoContainers/ItemId.h"
#include <map>
#include <memory>
#include <functional>

class QLayout;

namespace phobos { namespace widgets { namespace pitem {

class PhotoItem;

namespace utils {

using PhotoItemsContentMap = std::map<pcontainer::ItemId, std::unique_ptr<PhotoItem> >;

PhotoItemsContentMap recoverFromLayout(QLayout *layout, std::function<bool(int const layoutIdx)> const& predicate);

}}}} // namespace phobos::widgets::pitem::utils

#endif // WIDGETS_PHOTOITEM_RECOVERY_H
