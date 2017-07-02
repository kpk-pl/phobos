#ifndef ROWSERIESVIEW_H
#define ROWSERIESVIEW_H

#include "SeriesViewBase.h"
#include "Widgets/HorizontalScrollArea.h"
#include "ImageCache/CacheFwd.h"
#include "ImageProcessing/MetricsFwd.h"

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

private slots:
    void updateImage(QUuid seriesUuid, QString filename, QImage image);
    void updateMetrics(QUuid seriesUuid, QString filename, iprocess::MetricPtr metrics);

private:
    widgets::pitem::PhotoItem* findItemWidget(QUuid const& seriesUuid, std::string const& fileName) const;

    widgets::HorizontalScrollArea* scroll;
};

} // namespace phobos

#endif // ROWSERIESVIEW_H
