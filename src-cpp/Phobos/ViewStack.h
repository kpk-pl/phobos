#ifndef VIEWSTACK_H
#define VIEWSTACK_H

#include <boost/optional.hpp>
#include <QStackedWidget>
#include <QUuid>
#include "PhotoContainers/Set.h"
#include "ViewDescription.h"

namespace phobos {

class AllSeriesView;
class NumSeriesView;
class RowSeriesView;
class SeriesViewBase;

class ViewStack : public QStackedWidget
{
    Q_OBJECT

public:
    explicit ViewStack();
    void addPhotos(QStringList const& photos);

public slots:
    void handleSwitchView(ViewDescriptionPtr const& viewDesc);

private:
    pcontainer::SeriesPtr const& findRequestedSeries(ViewDescriptionPtr const& viewDesc) const;
    void updateCurrentWidget(ViewDescriptionPtr const& viewDesc);

    void setupUI();
    void connectSignals();

    pcontainer::Set seriesSet;
    boost::optional<QUuid> currentSeriesInView;

    AllSeriesView* allSeriesView;
    RowSeriesView* rowSeriesView;
    NumSeriesView* numSeriesView;
    SeriesViewBase* currentSeriesWidget;
};

} // namespace phobos

#endif // VIEWSTACK_H
