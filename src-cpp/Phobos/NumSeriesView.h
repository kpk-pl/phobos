#ifndef NUMSERIESVIEW_H
#define NUMSERIESVIEW_H

#include <vector>
#include "SeriesViewBase.h"
#include "ImageCache/CacheFwd.h"
#include "ImageProcessing/MetricsFwd.h"

namespace phobos {

class NumSeriesView : public SeriesViewBase
{
public:
    explicit NumSeriesView(icache::Cache const& imageCache);
    ~NumSeriesView();

    void showSeries(pcontainer::SeriesPtr const& series) override;
    void clear() override;

    void keyPressEvent(QKeyEvent* keyEvent) override;

protected:
    void addToLayout(widgets::pitem::PhotoItem* itemWidget) override;
    void changeSeriesState(pcontainer::ItemState const state) const override;

private slots:
    void updateImage(QUuid seriesUuid, QString filename, QImage image);
    void updateMetrics(QUuid seriesUuid, QString filename, iprocess::MetricPtr metrics);

private:
    void showNextItem();
    void showPrevItem();
    void setCurrentView();

    unsigned const visibleItems;
    unsigned currentItem;
    std::vector<widgets::pitem::PhotoItem*> photoItems;
    QLayout* layoutForItems;
};

} // namespace phobos

#endif // NUMSERIESVIEW_H
