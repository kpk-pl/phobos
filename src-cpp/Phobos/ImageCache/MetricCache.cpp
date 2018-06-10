#include "ImageCache/MetricCache.h"
#include "ImageProcessing/MetricsAggregate.h"
#include "ImageProcessing/MetricsIO.h"
#include "PhotoContainers/Set.h"
#include "Utils/Algorithm.h"
#include "ConfigExtension.h"
#include <easylogging++.h>

namespace phobos { namespace icache {

MetricCache::MetricCache(pcontainer::Set const& photoSet) :
  photoSet(photoSet)
{
  QObject::connect(&photoSet, &pcontainer::Set::changedSeries, this, &MetricCache::changedSeries);
}

bool MetricCache::has(pcontainer::ItemId const& itemId) const
{
  return utils::valueIn(itemId.fileName, metricCache);
}

iprocess::MetricPtr MetricCache::get(pcontainer::ItemId const& itemId) const
{
  auto const it = metricCache.find(itemId.fileName);
  if (it == metricCache.end())
    return nullptr;

  LOG(DEBUG) << "[Cache] Retrieved metrics for " << itemId.fileName;
  return it->second;
}

void MetricCache::newLoadedFromThread(pcontainer::ItemId itemId, iprocess::MetricPtr metrics)
{
  metricCache.emplace(itemId.fileName, metrics);
  LOG(DEBUG) << "[Cache] Saved new metrics for " << itemId.fileName;

  if (!changedSeries(itemId.seriesUuid))
    emit updateMetrics(itemId, metrics); // if not all metrics are loaded, emit just for the current one
}

bool MetricCache::changedSeries(QUuid const& seriesUuid)
{
  auto const& series = photoSet.findSeries(seriesUuid);
  if (!series.isPhotoSeries)
    return false;

  if (!std::all_of(series.begin(), series.end(),
        [this](pcontainer::ItemPtr const& item){
            return utils::valueIn(item->fileName(), metricCache);
        }))
  {
    return false;
  }

  auto allMetrics = utils::transformToVector<iprocess::MetricPtr>(series.begin(), series.end(),
      [this](auto const& item){ return metricCache[item->fileName()]; });

  LOG(DEBUG) << "[Cache] Aggregating metrics for series " << seriesUuid.toString();
  iprocess::aggregate(allMetrics);

  bool const doLog = config::qualified("logging.metrics", false);
  for (std::size_t i = 0; i < series.size(); ++i)
  {
    auto const& filename = series.item(i)->fileName();
    auto const& metric = metricCache[filename];
    emit updateMetrics(pcontainer::ItemId{seriesUuid, filename}, metric);

    LOG_IF(doLog, DEBUG) << "Calculated series metrics: "
       << "photoItem: " << filename << ' '
       << "metric: " << metric;
  }

  return true;
}

}} // namespace phobos::icache
