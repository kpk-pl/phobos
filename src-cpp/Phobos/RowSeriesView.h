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
    explicit RowSeriesView(pcontainer::Set const& seriesSet, icache::Cache & imageCache);

    void showSeries(pcontainer::SeriesPtr const& series) override;
    void clear() override;

private slots:
    void resizeImages(int percent);

protected:
    void addToLayout(std::unique_ptr<widgets::pitem::PhotoItem> itemWidget) override;
    void changeSeriesState(pcontainer::ItemState const state) const override;
    widgets::pitem::PhotoItem*
        findItemWidget(pcontainer::ItemId const& itemId) const override;

    void updateCurrentSeries() override;

private:
    widgets::HorizontalScrollArea* scroll;
};

} // namespace phobos

#endif // ROWSERIESVIEW_H
