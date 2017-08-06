#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include "NumSeriesView.h"
#include "NavigationBar.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Config.h"
#include "Utils/LayoutClear.h"
#include "ImageCache/Cache.h"

namespace phobos {

// TODO: Runtime configurable selection of items visible

NumSeriesView::NumSeriesView(pcontainer::Set const& seriesSet, icache::Cache const& imageCache) :
    SeriesViewBase(seriesSet, imageCache),
    visibleItems(config::get()->get_qualified_as<unsigned>("seriesView.num.visibleItems").value_or(2)),
    currentItem(0)
{
    NavigationBar* navigationBar = new NavigationBar(NavigationBar::Capability::ALL_SERIES |
                                                     NavigationBar::Capability::ONE_SERIES |
                                                     NavigationBar::Capability::LEFT |
                                                     NavigationBar::Capability::RIGHT);

    layoutForItems = new QHBoxLayout();

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget(navigationBar);
    vlayout->addLayout(layoutForItems);
    vlayout->addStretch();

    setLayout(vlayout);

    QObject::connect(navigationBar->allSeriesButton(), &QPushButton::clicked,
                     this, [this](){ switchView(ViewDescription::make(ViewType::ALL_SERIES, currentSeriesUuid)); });
    QObject::connect(navigationBar->oneSeriesButton(), &QPushButton::clicked,
                     this, [this](){ switchView(ViewDescription::make(ViewType::ROW_SINGLE_SERIES, currentSeriesUuid)); });
    QObject::connect(navigationBar->leftButton(), &QPushButton::clicked,
                     this, [this](){ switchView(ViewDescription::make(ViewType::NUM_SINGLE_SERIES, currentSeriesUuid, -1)); });
    QObject::connect(navigationBar->rightButton(), &QPushButton::clicked,
                     this, [this](){ switchView(ViewDescription::make(ViewType::NUM_SINGLE_SERIES, currentSeriesUuid, +1)); });
}

NumSeriesView::~NumSeriesView()
{
    utils::clearLayout(layoutForItems, false);
    for (QWidget* widget : photoItems)
        delete widget;
}

void NumSeriesView::showSeries(pcontainer::SeriesPtr const& series)
{
    SeriesViewBase::showSeries(series);
    currentItem = 0;
    layoutForItems->itemAt(0)->widget()->setFocus();
}

widgets::pitem::PhotoItem* NumSeriesView::findItemWidget(pcontainer::ItemId const& itemId) const
{
  if (currentSeriesUuid != itemId.seriesUuid)
      return nullptr;

  auto const widgetIt = std::find_if(photoItems.begin(), photoItems.end(),
      [&itemId](widgets::pitem::PhotoItem* const p){ return p->photoItem().id() == itemId; });

  assert(widgetIt != photoItems.end());
  return *widgetIt;
}

void NumSeriesView::clear()
{
    SeriesViewBase::clear();
    utils::clearLayout(layoutForItems, false);
    currentItem = 0;
    for (auto widgetPtr : photoItems)
        delete widgetPtr;

    photoItems.clear();
    update();
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
    if (currentItem > 0)
    {
        --currentItem;
        setCurrentView();
    }
}

void NumSeriesView::showNextItem()
{
    if (currentItem < photoItems.size()-1)
    {
        ++currentItem;
        setCurrentView();
    }
}

void NumSeriesView::setCurrentView()
{
    utils::clearLayout(layoutForItems, false);

    int startShow = std::max(int(currentItem) - int((visibleItems-1)/2), 0);
    int endShow = std::min(startShow + int(visibleItems), int(photoItems.size()));
    if (endShow - startShow < int(visibleItems))
        startShow = std::max(endShow - int(visibleItems), 0);

    for (int i = startShow; i < endShow; ++i)
        layoutForItems->addWidget(photoItems[i]);

    layoutForItems->itemAt(currentItem - startShow)->widget()->setFocus();
}

void NumSeriesView::addToLayout(widgets::pitem::PhotoItem* itemWidget)
{
    photoItems.push_back(itemWidget);
    if (layoutForItems->count() < int(visibleItems))
        layoutForItems->addWidget(itemWidget);
}

void NumSeriesView::updateCurrentSeries()
{
  clear();
  // TODO: Optimize this so that no clear is performed.
  pcontainer::SeriesPtr const& series = seriesSet.findSeries(*currentSeriesUuid);
  showSeries(series);
}

void NumSeriesView::changeSeriesState(pcontainer::ItemState const state) const
{
    for (widgets::pitem::PhotoItem *photoWidget : photoItems)
        photoWidget->photoItem().setState(state);
}

} // namespace phobos
