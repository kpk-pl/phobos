#ifndef VIEWS_VIEW_H
#define VIEWS_VIEW_H

#include "ImageCache/CacheFwd.h"
#include "PhotoContainers/Fwd.h"
#include <QWidget>

namespace phobos { namespace view {

class View : public QWidget
{
Q_OBJECT
public:
  explicit View(pcontainer::Set const& seriesSet, icache::Cache & imageCache);

protected:
  pcontainer::Set const& seriesSet;
  icache::Cache & imageCache;
};

}} // namespace phobos::view

#endif // VIEWS_VIEW_H
