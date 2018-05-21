#include "Views/RowSeries.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/Recovery.h"
#include "Utils/LayoutClear.h"
#include "Utils/Asserted.h"
#include "ImageCache/Cache.h"
#include <easylogging++.h>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QPushButton>
#include <QSlider>

namespace phobos { namespace view {

RowSeries::RowSeries(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
  SeriesBase(seriesSet, imageCache)
{
  scroll = new widgets::HorizontalScrollArea();
  scroll->boxLayout()->setContentsMargins(0, 0, 0, 0);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->addWidget(scroll, 100);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addStretch(0);
  setLayout(layout);
}

QLayout* RowSeries::getLayoutForItems() const
{
  return scroll->boxLayout();
}

void RowSeries::resizeImages(int percent)
{
  assert(percent >= 0 && percent <= 100);
  QVBoxLayout* vl = dynamic_cast<QVBoxLayout*>(layout());
  vl->setStretch(0, percent);
  vl->setStretch(1, 100-percent);
}

void RowSeries::clear()
{
  SeriesBase::clear();
  utils::clearLayout(scroll->boxLayout());
  scroll->horizontalScrollBar()->setValue(0);
}

widgets::pitem::PhotoItem* RowSeries::findItemWidget(pcontainer::ItemId const& itemId) const
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

  return utils::asserted::always;
}

void RowSeries::updateCurrentSeries()
{
  int const prevScrollValue = scroll->horizontalScrollBar()->value();

  auto oldContent = widgets::pitem::utils::recoverFromLayout(getLayoutForItems(), [](int){return true;});
  updateCurrentSeriesFromContent(oldContent);
  LOG(DEBUG) << "Updated current series " << currentSeriesUuid->toString();

  scroll->horizontalScrollBar()->setValue(prevScrollValue);
}

void RowSeries::changeSeriesState(pcontainer::ItemState const state) const
{
  for (int i = 0; i < scroll->boxLayout()->count(); ++i)
  {
    widgets::pitem::PhotoItem *photoWidget = dynamic_cast<widgets::pitem::PhotoItem*>(scroll->boxLayout()->itemAt(i)->widget());
    assert(photoWidget);
    photoWidget->photoItem().setState(state);
  }
}

}} // namespace phobos::view
