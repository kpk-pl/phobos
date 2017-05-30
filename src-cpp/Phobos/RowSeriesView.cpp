#include <QVBoxLayout>
#include <QScrollBar>
#include "RowSeriesView.h"
#include "NavigationBar.h"
#include "Utils/LayoutClear.h"
#include "PhotoItemWidget.h"
#include "ImageCache/Cache.h"

namespace phobos {

RowSeriesView::RowSeriesView(icache::Cache const& imageCache) :
    SeriesViewBase(imageCache)
{
    QObject::connect(&imageCache, &icache::Cache::updateImage, this, &RowSeriesView::updateImage);

    NavigationBar* navigationBar = new NavigationBar(NavigationBar::Capability::ALL_SERIES |
                                                     NavigationBar::Capability::NUM_SERIES |
                                                     NavigationBar::Capability::SLIDER |
                                                     NavigationBar::Capability::LEFT |
                                                     NavigationBar::Capability::RIGHT);

    scroll = new HorizontalScrollArea();
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

void RowSeriesView::updateImage(QUuid seriesUuid, std::string filename, QImage image)
{
    if (currentSeriesUuid != seriesUuid)
        return;

    for (int i = 0; i < scroll->boxLayout()->count(); ++i)
    {
        auto const photoWidget = dynamic_cast<PhotoItemWidget*>(scroll->boxLayout()->itemAt(i)->widget());
        assert(photoWidget);

        if (photoWidget->photoItem().fileName() == filename)
        {
            photoWidget->setImage(image);
            return;
        }
    }

    assert(false); // impossible
}

void RowSeriesView::addToLayout(PhotoItemWidget* itemWidget)
{
    scroll->boxLayout()->addWidget(itemWidget);
}

//std::vector<PhotoItemWidget*> RowSeriesView::moveItemsOut()
//{
    //std::vector<PhotoItemWidget*> result;
    //result.reserve(scroll->boxLayout()->count());
    //for (int i = 0; i < scroll->boxLayout()->count(); ++i)
    //{
        //PhotoItemWidget *photoWidget = dynamic_cast<PhotoItemWidget*>(scroll->boxLayout()->itemAt(i)->widget());
        //assert(photoWidget);
        //photoWidget->disconnect(this);
        //result.push_back(photoWidget);
    //}

    //utils::clearLayout(scroll->boxLayout(), false);
    //currentSeriesUuid.reset();
    //scroll->horizontalScrollBar()->setValue(0);
    //return result;
//}

void RowSeriesView::changeSeriesState(pcontainer::ItemState const state) const
{
    for (int i = 0; i < scroll->boxLayout()->count(); ++i)
    {
        PhotoItemWidget *photoWidget = dynamic_cast<PhotoItemWidget*>(scroll->boxLayout()->itemAt(i)->widget());
        assert(photoWidget);
        photoWidget->photoItem().setState(state);
    }
}

} // namespace phobos
