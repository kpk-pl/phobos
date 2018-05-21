#include "Views/NumSeries.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Config.h"
#include "Utils/LayoutClear.h"
#include "ImageCache/Cache.h"
#include <easylogging++.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QPushButton>

namespace phobos { namespace view {

// TODO: Runtime configurable selection of items visible

NumSeries::NumSeries(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
  SeriesBase(seriesSet, imageCache),
  visibleItems(config::get()->get_qualified_as<unsigned>("seriesView.num.visibleItems").value_or(2)),
  currentItem(0)
{
  layoutForItems = new QHBoxLayout();

  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->addLayout(layoutForItems);
  vlayout->addStretch();

  setLayout(vlayout);
}

NumSeries::~NumSeries()
{
  utils::clearLayout(layoutForItems, false);
}

QLayout* NumSeries::getLayoutForItems() const
{
  return layoutForItems;
}

void NumSeries::showSeries(pcontainer::Series const& series)
{
  SeriesBase::showSeries(series);
  currentItem = 0;
}

widgets::pitem::PhotoItem* NumSeries::findItemWidget(pcontainer::ItemId const& itemId) const
{
  if (currentSeriesUuid != itemId.seriesUuid)
    return nullptr;

  auto const widgetIt = std::find_if(photoItems.begin(), photoItems.end(),
      [&itemId](auto const& p){ return p->photoItem().id() == itemId; });

  assert(widgetIt != photoItems.end());
  return widgetIt->get();
}

void NumSeries::clear()
{
  SeriesBase::clear();
  utils::clearLayout(layoutForItems, false);
  photoItems.clear();
  currentItem = 0;
}

void NumSeries::keyPressEvent(QKeyEvent* keyEvent)
{
  if (keyEvent->key() == Qt::Key_Left)
    showPrevItem();
  else if (keyEvent->key() == Qt::Key_Right)
    showNextItem();

  SeriesBase::keyPressEvent(keyEvent);
}

void NumSeries::showPrevItem()
{
  VisibleRange range;
  if (currentItem > 0)
  {
    --currentItem;
    range = visibleRange();
    setCurrentView(range);
  }
  else
  {
    range = visibleRange();
  }

  focusCurrentItem(range);
}

void NumSeries::showNextItem()
{
  VisibleRange range;
  if (currentItem < photoItems.size()-1)
  {
    ++currentItem;
    range = visibleRange();
    setCurrentView(range);
  }
  else
  {
    range = visibleRange();
  }

  focusCurrentItem(range);
}

NumSeries::VisibleRange NumSeries::visibleRange() const
{
  int startShow = std::max(int(currentItem) - int((visibleItems-1)/2), 0);
  int endShow = std::min(startShow + int(visibleItems), int(photoItems.size()));
  if (endShow - startShow < int(visibleItems))
      startShow = std::max(endShow - int(visibleItems), 0);

  return std::make_pair(startShow, endShow);
}

void NumSeries::setCurrentView(VisibleRange const& range)
{
  utils::clearLayout(layoutForItems, false);

  for (int i = range.first; i < range.second; ++i)
    layoutForItems->addWidget(photoItems[i].get());

}

void NumSeries::focusCurrentItem(VisibleRange const& range)
{
  layoutForItems->itemAt(currentItem - range.first)->widget()->setFocus();
}

void NumSeries::addToLayout(std::unique_ptr<widgets::pitem::PhotoItem> itemWidget)
{
  if (layoutForItems->count() < int(visibleItems))
    layoutForItems->addWidget(itemWidget.get());

  photoItems.push_back(std::move(itemWidget));
}

void NumSeries::updateCurrentSeries()
{
  utils::clearLayout(layoutForItems, false);

  std::map<pcontainer::ItemId, std::unique_ptr<widgets::pitem::PhotoItem>> oldContent;
  for (auto &item : photoItems)
  {
    auto const id = item->photoItem().id();
    oldContent.emplace(id, std::move(item));
  }
  photoItems.clear();

  updateCurrentSeriesFromContent(oldContent);
  LOG(DEBUG) << "Updated current series " << currentSeriesUuid->toString();

  if (layoutForItems->count() > 0)
    currentItem = std::min(currentItem, static_cast<unsigned>(layoutForItems->count()-1));
  else
    currentItem = 0;
}

void NumSeries::changeSeriesState(pcontainer::ItemState const state) const
{
  for (auto const& photoWidget : photoItems)
    photoWidget->photoItem().setState(state);
}

}} // namespace phobos::view
