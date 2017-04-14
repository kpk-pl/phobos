#ifndef SERIESVIEWBASE_H
#define SERIESVIEWBASE_H

#include <QWidget>
#include <QUuid>
#include <QMetaObject>
#include <boost/optional.hpp>
#include "PhotoContainers/Series.h"
#include "ViewDescription.h"

namespace phobos {

class PhotoItemWidget;

class SeriesViewBase : public QWidget
{
    Q_OBJECT
public:
    explicit SeriesViewBase() = default;
    virtual ~SeriesViewBase() = default;

    virtual void showSeries(pcontainer::SeriesPtr const& series);
    virtual void clear();
    void exchangeItemsFrom(SeriesViewBase *source);

signals:
    void switchView(ViewDescriptionPtr const& viewDesc);

protected:
    virtual void addToLayout(PhotoItemWidget* itemWidget) = 0;
    virtual std::vector<PhotoItemWidget*> moveItemsOut() = 0;
    virtual void moveItemsIn(std::vector<PhotoItemWidget*> const& items);

    boost::optional<QUuid> currentSeriesUuid;
};

} // namespace phobos

#endif // SERIESVIEWBASE_H
