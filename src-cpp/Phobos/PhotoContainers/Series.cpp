#include <algorithm>
#include "PhotoContainers/Series.h"
#include "ImageProcessing/MetricsAggregate.h"

namespace phobos { namespace pcontainer {

Series::Series() :
    _uuid(QUuid::createUuid())
{

}

void Series::addPhotoItems(std::vector<std::string> const& fileNames)
{
    photoItems.reserve(photoItems.size() + fileNames.size());
    for (auto const& fn : fileNames)
        addPhotoItem(fn);
}

void Series::addPhotoItem(std::string const& fileName)
{
    auto newItem = std::make_shared<Item>(fileName, _uuid);
    QObject::connect(newItem.get(), &Item::metricsReady, this, &Series::newMetricCalculated);
    photoItems.emplace_back(std::move(newItem));
}

void Series::newMetricCalculated()
{
    using namespace iprocess;

    MetricPtrVec allMetrics;
    allMetrics.reserve(photoItems.size());
    std::transform(photoItems.begin(), photoItems.end(), std::back_inserter(allMetrics),
                   [](ItemPtr const& item){ return item->metric();} );

    if (!std::all_of(allMetrics.begin(), allMetrics.end(),
                     [](MetricPtr const& m){ return bool(m); }))
        return;

    ScoredMetricPtrVec scoredMetrics = aggregateMetrics(allMetrics);
    for (std::size_t i = 0; i < photoItems.size(); ++i)
        photoItems[i]->setScoredMetric(scoredMetrics[i]);
}

}} // namespace phobos::pcontainer
