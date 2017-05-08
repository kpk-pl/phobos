#ifndef ROWSERIESVIEW_H
#define ROWSERIESVIEW_H

#include "SeriesViewBase.h"
#include "HorizontalScrollArea.h"

namespace phobos {

class RowSeriesView : public SeriesViewBase
{
    Q_OBJECT
public:
    explicit RowSeriesView();

    void showSeries(pcontainer::SeriesPtr const& series) override;
    void clear() override;

private slots:
    void resizeImages(int percent);

protected:
    void addToLayout(PhotoItemWidget* itemWidget) override;
    std::vector<PhotoItemWidget*> moveItemsOut() override;
    void changeSeriesState(pcontainer::ItemState const state) const override;
    void moveItemsIn(std::vector<PhotoItemWidget*> const& items) override;

private:
    HorizontalScrollArea* scroll;
};

} // namespace phobos

#endif // ROWSERIESVIEW_H
