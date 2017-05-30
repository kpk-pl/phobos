#ifndef NUMSERIESVIEW_H
#define NUMSERIESVIEW_H

#include <vector>
#include "SeriesViewBase.h"
#include "ImageCache/CacheFwd.h"

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
    void addToLayout(PhotoItemWidget* itemWidget) override;
    void changeSeriesState(pcontainer::ItemState const state) const override;

private slots:
    void updateImage(QUuid seriesUuid, std::string filename, QImage image);

private:
    void showNextItem();
    void showPrevItem();
    void setCurrentView();

    unsigned const visibleItems;
    unsigned currentItem;
    std::vector<PhotoItemWidget*> photoItems;
    QLayout* layoutForItems;
};

} // namespace phobos

#endif // NUMSERIESVIEW_H
