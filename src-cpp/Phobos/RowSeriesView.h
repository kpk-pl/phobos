#ifndef ROWSERIESVIEW_H
#define ROWSERIESVIEW_H

#include "SeriesViewBase.h"
#include "Widgets/HorizontalScrollArea.h"
#include "ImageCache/CacheFwd.h"

namespace phobos {

class RowSeriesView : public SeriesViewBase
{
    Q_OBJECT
public:
    explicit RowSeriesView(icache::Cache const& imageCache);

    void showSeries(pcontainer::SeriesPtr const& series) override;
    void clear() override;

private slots:
    void resizeImages(int percent);

protected:
    void addToLayout(widgets::pitem::PhotoItem* itemWidget) override;
    void changeSeriesState(pcontainer::ItemState const state) const override;

private:
    widgets::pitem::PhotoItem* findItemWidget(pcontainer::ItemId const& itemId) const override;

    widgets::HorizontalScrollArea* scroll;
};

} // namespace phobos

#endif // ROWSERIESVIEW_H
