#ifndef ROWSERIESVIEW_H
#define ROWSERIESVIEW_H

#include "Views/SeriesBase.h"
#include "Widgets/HorizontalScrollArea.h"

namespace phobos { namespace view {

class RowSeries : public SeriesBase
{
  Q_OBJECT

public:
  explicit RowSeries(pcontainer::Set const& seriesSet, icache::Cache & imageCache);

  void clear() override;

public slots:
  void resizeImages(int percent);

protected:
  void changeSeriesState(pcontainer::ItemState const state) const override;
  widgets::pitem::PhotoItem*
      findItemWidget(pcontainer::ItemId const& itemId) const override;

  void updateCurrentSeries() override;
  QLayout* getLayoutForItems() const override;

private:
  widgets::HorizontalScrollArea* scroll;
};

}} // namespace phobos::view

#endif // ROWSERIESVIEW_H
