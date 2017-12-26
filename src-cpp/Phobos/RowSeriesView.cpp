#include "RowSeriesView.h"
#include "NavigationBar.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/Recovery.h"
#include "Utils/LayoutClear.h"
#include "ImageCache/Cache.h"
#include <easylogging++.h>
#include <QVBoxLayout>
#include <QScrollBar>

namespace phobos {

RowSeriesView::RowSeriesView(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
  SeriesViewBase(seriesSet, imageCache)
{
  NavigationBar* navigationBar = new NavigationBar(NavigationBar::Capability::ALL_SERIES |
                                                   NavigationBar::Capability::NUM_SERIES |
                                                   NavigationBar::Capability::SLIDER |
                                                   NavigationBar::Capability::LEFT |
                                                   NavigationBar::Capability::RIGHT);

  scroll = new widgets::HorizontalScrollArea();
  scroll->boxLayout()->setContentsMargins(0, 0, 0, 0);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->addWidget(navigationBar);
  layout->addWidget(scroll, 100);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addStretch(0);
  setLayout(layout);

  QObject::connect(navigationBar->allSeriesButton(), &QPushButton::clicked,
                   this, [this](){ switchView(ViewDescription::make(ViewType::ALL_SERIES, currentSeriesUuid)); });
  QObject::connect(navigationBar->numSeriesButton(), &QPushButton::clicked,
                   this, [this](){ switchView(ViewDescription::make(ViewType::NUM_SINGLE_SERIES, currentSeriesUuid)); });
  QObject::connect(navigationBar->slider(), &QSlider::valueChanged,
                   this, &RowSeriesView::resizeImages);
  QObject::connect(navigationBar->leftButton(), &QPushButton::clicked,
                   this, [this](){ switchView(ViewDescription::make(ViewType::ROW_SINGLE_SERIES, currentSeriesUuid, -1)); });
  QObject::connect(navigationBar->rightButton(), &QPushButton::clicked,
                   this, [this](){ switchView(ViewDescription::make(ViewType::ROW_SINGLE_SERIES, currentSeriesUuid, +1)); });
}

QLayout* RowSeriesView::getLayoutForItems() const
{
  return scroll->boxLayout();
}

void RowSeriesView::resizeImages(int percent)
{
  assert(percent >= 0 && percent <= 100);
  QVBoxLayout* vl = dynamic_cast<QVBoxLayout*>(layout());
  vl->setStretch(1, percent);
  vl->setStretch(2, 100-percent);
}

void RowSeriesView::clear()
{
  SeriesViewBase::clear();
  utils::clearLayout(scroll->boxLayout());
  scroll->horizontalScrollBar()->setValue(0);
  update();
}

widgets::pitem::PhotoItem* RowSeriesView::findItemWidget(pcontainer::ItemId const& itemId) const
{
  if (currentSeriesUuid != itemId.seriesUuid)
    return nullptr;

  for (int i = 0; i < scroll->boxLayout()->count(); ++i)
  {
    auto const photoWidget = dynamic_cast<widgets::pitem::PhotoItem*>(scroll->boxLayout()->itemAt(i)->widget());
    assert(photoWidget);

    if (photoWidget->photoItem().id() == itemId)
      return photoWidget;
  }

  assert(false); // impossible
  return nullptr;
}

void RowSeriesView::updateCurrentSeries()
{
  int const prevScrollValue = scroll->horizontalScrollBar()->value();

  auto oldContent = widgets::pitem::utils::recoverFromLayout(getLayoutForItems(), [](int){return true;});
  updateCurrentSeriesFromContent(oldContent);
  LOG(DEBUG) << "Updated current series " << currentSeriesUuid->toString();

  scroll->horizontalScrollBar()->setValue(prevScrollValue);
}

void RowSeriesView::changeSeriesState(pcontainer::ItemState const state) const
{
  for (int i = 0; i < scroll->boxLayout()->count(); ++i)
  {
    widgets::pitem::PhotoItem *photoWidget = dynamic_cast<widgets::pitem::PhotoItem*>(scroll->boxLayout()->itemAt(i)->widget());
    assert(photoWidget);
    photoWidget->photoItem().setState(state);
  }
}

} // namespace phobos
