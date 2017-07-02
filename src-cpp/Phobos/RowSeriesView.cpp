#include <QVBoxLayout>
#include <QScrollBar>
#include "RowSeriesView.h"
#include "NavigationBar.h"
#include "Utils/LayoutClear.h"
#include "Widgets/PhotoItem/PhotoItem.h"
#include "ImageCache/Cache.h"

namespace phobos {

RowSeriesView::RowSeriesView(icache::Cache const& imageCache) :
    SeriesViewBase(imageCache)
{
    QObject::connect(&imageCache, &icache::Cache::updateImage, this, &RowSeriesView::updateImage);
    QObject::connect(&imageCache, &icache::Cache::updateMetrics, this, &RowSeriesView::updateMetrics);

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

void RowSeriesView::showSeries(pcontainer::SeriesPtr const& series)
{
    SeriesViewBase::showSeries(series);
    scroll->boxLayout()->itemAt(0)->widget()->setFocus();
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

void RowSeriesView::updateImage(QUuid seriesUuid, QString filename, QImage image)
{
  widgets::pitem::PhotoItem* item = findItemWidget(seriesUuid, filename.toStdString());
  if (item)
    item->setImage(image);
}

void RowSeriesView::updateMetrics(QUuid seriesUuid, QString filename, iprocess::MetricPtr metrics)
{
  widgets::pitem::PhotoItem* item = findItemWidget(seriesUuid, filename.toStdString());
  if (item)
    item->setMetrics(metrics);
}

widgets::pitem::PhotoItem* RowSeriesView::findItemWidget(QUuid const& seriesUuid, std::string const& fileName) const
{
    if (currentSeriesUuid != seriesUuid)
        return nullptr;

    for (int i = 0; i < scroll->boxLayout()->count(); ++i)
    {
        auto const photoWidget = dynamic_cast<widgets::pitem::PhotoItem*>(scroll->boxLayout()->itemAt(i)->widget());
        assert(photoWidget);

        if (photoWidget->photoItem().fileName() == fileName)
          return photoWidget;
    }

    assert(false); // impossible
    return nullptr;
}

void RowSeriesView::addToLayout(widgets::pitem::PhotoItem* itemWidget)
{
    scroll->boxLayout()->addWidget(itemWidget);
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
