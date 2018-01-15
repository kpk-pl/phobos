#include "NumSeriesView.h"
#include "Widgets/NavigationBar.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Config.h"
#include "Utils/LayoutClear.h"
#include "ImageCache/Cache.h"
#include <easylogging++.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QPushButton>

namespace phobos {

// TODO: Runtime configurable selection of items visible

NumSeriesView::NumSeriesView(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
    SeriesViewBase(seriesSet, imageCache),
    visibleItems(config::get()->get_qualified_as<unsigned>("seriesView.num.visibleItems").value_or(2)),
    currentItem(0)
{
  widgets::NavigationBar* navigationBar = new widgets::NavigationBar();

  navigationBar->addButton("allSeries")->setToolTip(tr("Return to main view with all series displayed"));
  navigationBar->addButton("oneSeries")->setToolTip(tr("Switch to view with whole series in one row"));
  navigationBar->addStretch();
  navigationBar->addButton("prevSeries")->setToolTip(tr("Previous series"));
  navigationBar->addButton("prevItem")->setToolTip(tr("Previous photo"));
  navigationBar->addButton("nextItem")->setToolTip(tr("Next photo"));
  navigationBar->addButton("nextSeries")->setToolTip(tr("Next series"));
  navigationBar->setContentsMargins(0, 0, 0, 0);

  layoutForItems = new QHBoxLayout();

  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->addWidget(navigationBar);
  vlayout->addLayout(layoutForItems);
  vlayout->addStretch();

  setLayout(vlayout);

  QObject::connect(navigationBar->button("allSeries"), &QPushButton::clicked,
                   this, [this](){ switchView(ViewDescription::make(ViewType::ALL_SERIES, currentSeriesUuid)); });
  QObject::connect(navigationBar->button("oneSeries"), &QPushButton::clicked,
                   this, [this](){ switchView(ViewDescription::make(ViewType::ROW_SINGLE_SERIES, currentSeriesUuid)); });
  QObject::connect(navigationBar->button("prevSeries"), &QPushButton::clicked,
                   this, [this](){ switchView(ViewDescription::make(ViewType::NUM_SINGLE_SERIES, currentSeriesUuid, -1)); });
  QObject::connect(navigationBar->button("nextSeries"), &QPushButton::clicked,
                   this, [this](){ switchView(ViewDescription::make(ViewType::NUM_SINGLE_SERIES, currentSeriesUuid, +1)); });
  QObject::connect(navigationBar->button("prevItem"), &QPushButton::clicked, this, &NumSeriesView::showPrevItem);
  QObject::connect(navigationBar->button("nextItem"), &QPushButton::clicked, this, &NumSeriesView::showNextItem);
}

NumSeriesView::~NumSeriesView()
{
  utils::clearLayout(layoutForItems, false);
}

QLayout* NumSeriesView::getLayoutForItems() const
{
  return layoutForItems;
}

void NumSeriesView::showSeries(pcontainer::Series const& series)
{
  SeriesViewBase::showSeries(series);
  currentItem = 0;
}

widgets::pitem::PhotoItem* NumSeriesView::findItemWidget(pcontainer::ItemId const& itemId) const
{
  if (currentSeriesUuid != itemId.seriesUuid)
    return nullptr;

  auto const widgetIt = std::find_if(photoItems.begin(), photoItems.end(),
      [&itemId](auto const& p){ return p->photoItem().id() == itemId; });

  assert(widgetIt != photoItems.end());
  return widgetIt->get();
}

void NumSeriesView::clear()
{
  SeriesViewBase::clear();
  utils::clearLayout(layoutForItems, false);
  photoItems.clear();
  currentItem = 0;
}

void NumSeriesView::keyPressEvent(QKeyEvent* keyEvent)
{
  if (keyEvent->key() == Qt::Key_Left)
    showPrevItem();
  else if (keyEvent->key() == Qt::Key_Right)
    showNextItem();

  SeriesViewBase::keyPressEvent(keyEvent);
}

void NumSeriesView::showPrevItem()
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

void NumSeriesView::showNextItem()
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

NumSeriesView::VisibleRange NumSeriesView::visibleRange() const
{
  int startShow = std::max(int(currentItem) - int((visibleItems-1)/2), 0);
  int endShow = std::min(startShow + int(visibleItems), int(photoItems.size()));
  if (endShow - startShow < int(visibleItems))
      startShow = std::max(endShow - int(visibleItems), 0);

  return std::make_pair(startShow, endShow);
}

void NumSeriesView::setCurrentView(VisibleRange const& range)
{
  utils::clearLayout(layoutForItems, false);

  for (int i = range.first; i < range.second; ++i)
    layoutForItems->addWidget(photoItems[i].get());

}

void NumSeriesView::focusCurrentItem(VisibleRange const& range)
{
  layoutForItems->itemAt(currentItem - range.first)->widget()->setFocus();
}

void NumSeriesView::addToLayout(std::unique_ptr<widgets::pitem::PhotoItem> itemWidget)
{
  if (layoutForItems->count() < int(visibleItems))
    layoutForItems->addWidget(itemWidget.get());

  photoItems.push_back(std::move(itemWidget));
}

void NumSeriesView::updateCurrentSeries()
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

void NumSeriesView::changeSeriesState(pcontainer::ItemState const state) const
{
  for (auto const& photoWidget : photoItems)
    photoWidget->photoItem().setState(state);
}

} // namespace phobos
