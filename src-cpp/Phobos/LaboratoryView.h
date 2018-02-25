#ifndef LABORATORYVIEW_H
#define LABORATORYVIEW_H

#include "PhotoContainers/Item.h"
#include "ImageCache/CacheFwd.h"
#include <QWidget>

namespace phobos {

namespace widgets {
class ImageWidget;
} // namespace widgets

// TODO: common class with seriesSet and cache there in protected

class LaboratoryView : public QWidget
{
Q_OBJECT
public:
  explicit LaboratoryView(pcontainer::Set const& seriesSet, icache::Cache & imageCache);

  void showItem(pcontainer::Item const& item);
  boost::optional<pcontainer::ItemId> const& currentItem() const { return currentId; }

private:
  pcontainer::Set const& seriesSet;
  icache::Cache & imageCache;

  widgets::ImageWidget *imageWidget;

  boost::optional<pcontainer::ItemId> currentId;
};

} // namespace phobos

#endif // LABORATORYVIEW_H
