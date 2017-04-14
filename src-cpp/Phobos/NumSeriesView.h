#ifndef NUMSERIESVIEW_H
#define NUMSERIESVIEW_H

#include <vector>
#include "SeriesViewBase.h"

namespace phobos {

class NumSeriesView : public SeriesViewBase
{
public:
    explicit NumSeriesView();
    ~NumSeriesView();

    void showSeries(pcontainer::SeriesPtr const& series) override;
    void clear() override;

    void keyPressEvent(QKeyEvent* keyEvent) override;

protected:
    void addToLayout(PhotoItemWidget* itemWidget) override;
    std::vector<PhotoItemWidget*> moveItemsOut() override;
    void moveItemsIn(std::vector<PhotoItemWidget*> const& items) override;

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
