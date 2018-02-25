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

private:
  pcontainer::Set const& seriesSet;
  icache::Cache & imageCache;

  widgets::ImageWidget *imageWidget;
};

} // namespace phobos

#endif // LABORATORYVIEW_H
