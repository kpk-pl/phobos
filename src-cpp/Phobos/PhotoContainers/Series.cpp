#include <algorithm>
#include <easylogging++.h>
#include "PhotoContainers/Series.h"
#include "ImageProcessing/MetricsAggregate.h"
#include "ImageProcessing/MetricsIO.h"

namespace phobos { namespace pcontainer {

Series::Series() :
    _uuid(QUuid::createUuid())
{
}

Series::Series(importwiz::PhotoSeries const& series) :
    Series()
{
    for (auto const& item : series)
        addPhotoItem(item.fileName);
}

void Series::addPhotoItems(std::vector<std::string> const& fileNames)
{
    photoItems.reserve(photoItems.size() + fileNames.size());
    for (auto const& fn : fileNames)
        addPhotoItem(fn);
}

void Series::addPhotoItem(std::string const& fileName)
{
    auto newItem = std::make_shared<Item>(fileName, _uuid, photoItems.size());
    QObject::connect(newItem.get(), &Item::metricsReady, this, &Series::newMetricCalculated);
    photoItems.emplace_back(std::move(newItem));
}

ItemPtr Series::best() const
{
    for (auto const& item : photoItems)
        if (item->scoredMetric() && item->scoredMetric()->bestQuality)
            return item;

    return nullptr;
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
    {
        Item& item = *photoItems[i];
        item.setScoredMetric(scoredMetrics[i]);
        LOG(DEBUG) << "Calculated series metrics" << std::endl
                   << "photoItem: " << item.fileName() << std::endl
                   << "metric: " << item.metric() << std::endl
                   << "scoredMetric: " << item.scoredMetric();
    }
}

}} // namespace phobos::pcontainer
