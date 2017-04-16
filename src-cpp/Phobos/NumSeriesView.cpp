#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include "NumSeriesView.h"
#include "NavigationBar.h"
#include "PhotoItemWidget.h"
#include "Config.h"
#include "Utils/LayoutClear.h"

namespace phobos {

// TODO: Runtime configurable selection of items visible

NumSeriesView::NumSeriesView() :
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

void NumSeriesView::moveItemsIn(std::vector<PhotoItemWidget*> const& items)
{
    SeriesViewBase::moveItemsIn(items);
    currentItem = 0;
    layoutForItems->itemAt(0)->widget()->setFocus();
}

void NumSeriesView::clear()
{
    SeriesViewBase::clear();
    utils::clearLayout(layoutForItems, false);
    currentItem = 0;
    for (QWidget* widget : photoItems)
        delete widget;
    photoItems.clear();
    update();
}

std::vector<PhotoItemWidget*> NumSeriesView::moveItemsOut()
{
    utils::clearLayout(layoutForItems, false);
    currentSeriesUuid.reset();
    currentItem = 0;

    std::vector<PhotoItemWidget*> result(std::move(photoItems));
    photoItems.clear();

    return result;
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

void NumSeriesView::addToLayout(PhotoItemWidget* itemWidget)
{
    photoItems.push_back(itemWidget);
    if (layoutForItems->count() < int(visibleItems))
        layoutForItems->addWidget(itemWidget);
}

} // namespace phobos
