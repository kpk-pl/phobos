#ifndef NUMSERIESVIEW_H
#define NUMSERIESVIEW_H

#include "SeriesViewBase.h"
#include "ImageCache/CacheFwd.h"
#include <vector>

namespace phobos {

class NumSeriesView : public SeriesViewBase
{
public:
  explicit NumSeriesView(pcontainer::Set const& seriesSet, icache::Cache & imageCache);
  ~NumSeriesView();

  void showSeries(pcontainer::Series const& series) override;
  void clear() override;

  void keyPressEvent(QKeyEvent* keyEvent) override;

protected:
  void addToLayout(std::unique_ptr<widgets::pitem::PhotoItem> itemWidget) override;
  void changeSeriesState(pcontainer::ItemState const state) const override;
  widgets::pitem::PhotoItem*
      findItemWidget(pcontainer::ItemId const& itemId) const override;

  void updateCurrentSeries() override;
  QLayout* getLayoutForItems() const override;

private:
  void showNextItem();
  void showPrevItem();

  using VisibleRange = std::pair<int, int>;
  VisibleRange visibleRange() const;
  void setCurrentView(VisibleRange const& range);
  void focusCurrentItem(VisibleRange const& range);

  unsigned const visibleItems;
  unsigned currentItem;
  std::vector<std::unique_ptr<widgets::pitem::PhotoItem>> photoItems;
  QLayout* layoutForItems;
};

} // namespace phobos

#endif // NUMSERIESVIEW_H
