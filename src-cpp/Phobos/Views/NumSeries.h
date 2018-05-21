#ifndef NUMSERIESVIEW_H
#define NUMSERIESVIEW_H

#include "Views/SeriesBase.h"
#include <vector>

namespace phobos { namespace view {

class NumSeries : public SeriesBase
{
public:
  explicit NumSeries(pcontainer::Set const& seriesSet, icache::Cache & imageCache);
  ~NumSeries() override;

  void showSeries(pcontainer::Series const& series) override;
  void clear() override;

  void keyPressEvent(QKeyEvent* keyEvent) override;

public slots:
  void showNextItem();
  void showPrevItem();

protected:
  void addToLayout(std::unique_ptr<widgets::pitem::PhotoItem> itemWidget) override;
  void changeSeriesState(pcontainer::ItemState const state) const override;
  widgets::pitem::PhotoItem*
      findItemWidget(pcontainer::ItemId const& itemId) const override;

  void updateCurrentSeries() override;
  QLayout* getLayoutForItems() const override;

private:
  using VisibleRange = std::pair<int, int>;
  VisibleRange visibleRange() const;
  void setCurrentView(VisibleRange const& range);
  void focusCurrentItem(VisibleRange const& range);

  unsigned const visibleItems;
  unsigned currentItem;
  std::vector<std::unique_ptr<widgets::pitem::PhotoItem>> photoItems;
  QLayout* layoutForItems;
};

}} // namespace phobos::view

#endif // NUMSERIESVIEW_H
