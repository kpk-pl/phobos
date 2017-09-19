#include "Widgets/PhotoItem/Recovery.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Utils/LayoutClear.h"
#include <QLayout>
#include <cassert>

namespace phobos { namespace widgets { namespace pitem { namespace utils {

PhotoItemsContentMap recoverFromLayout(QLayout *layout, std::function<bool(int const layoutIdx)> const& predicate)
{
  PhotoItemsContentMap result;

  for (int idx = 0; idx < layout->count(); /* none */)
  {
    if (!predicate(idx))
    {
      ++idx;
      continue;
    }

    QLayoutItem *layoutItem = layout->takeAt(idx);
    assert(layoutItem);

    std::unique_ptr<widgets::pitem::PhotoItem> photoItem(dynamic_cast<widgets::pitem::PhotoItem*>(layoutItem->widget()));
    if (!photoItem)
    {
      phobos::utils::clearLayoutItem(layoutItem);
      continue;
    }

    result.emplace(photoItem->photoItem().id(), std::move(photoItem));
    phobos::utils::clearLayoutItem(layoutItem, false);
  }

  return result;
}

}}}} // namespace phobos::widgets::pitem::utils
