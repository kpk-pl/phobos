#include <easylogging++.h>
#include <QThreadPool>
#include "PhotoContainers/Series.h"
#include "ImageCache/Cache.h"
#include "Utils/Algorithm.h"
#include "Utils/Preload.h"
#include "Utils/Asserted.h"
#include "ConfigExtension.h"
#include "ImageProcessing/MetricsAggregate.h"
#include "ImageProcessing/MetricsIO.h"

// TODO: Maybe use transaction-like logic and log TRXID

namespace phobos { namespace icache {

Cache::Cache(pcontainer::Set const& photoSet) :
    photoSet(photoSet)
{
}

namespace {
  QImage getInitialPreload()
  {
    static QImage const preloadImage =
      utils::preloadImage(config::qSize("imageCache.preloadSize", QSize(320, 240)));

    return preloadImage;
  }
} // unnamed namespace

QImage Cache::getImage(pcontainer::ItemId const& itemId) const
{
  LOG(DEBUG) << "[Cache] Requested full image for " << itemId.fileName;
  QImage const fullImage = fullImageCache.find(itemId.fileName);
  if (!fullImage.isNull())
  {
    LOG(DEBUG) << "[Cache] Returned full image for " << itemId.fileName;
    return fullImage;
  }

  startThreadForItem(itemId);

  auto it = preloadImageCache.find(itemId.fileName);
  if (it != preloadImageCache.end() && !it->second.isNull())
  {
    LOG(DEBUG) << "[Cache] Returned preload image for " << itemId.fileName;
    return it->second;
  }
  else
  {
    LOG(DEBUG) << "[Cache] Returned initial placeholder image for " << itemId.fileName;
    return getInitialPreload();
  }
}

QImage Cache::getPreload(pcontainer::ItemId const& itemId) const
{
  LOG(DEBUG) << "[Cache] Requested preload image for " << itemId.fileName;
  auto it = preloadImageCache.find(itemId.fileName);
  if (it != preloadImageCache.end() && !it->second.isNull())
  {
    LOG(DEBUG) << "[Cache] Returned preload image for " << itemId.fileName;
    return it->second;
  }

  startThreadForItem(itemId);

  LOG(DEBUG) << "[Cache] Returned initial placeholder image for " << itemId.fileName;
  return getInitialPreload();
}

std::unique_ptr<iprocess::LoaderThread> Cache::makeLoadingThread(pcontainer::ItemId const& itemId) const
{
  QSize const fullSize = config::qSize("imageCache.fullSize", QSize(1920, 1080));
  auto thread = std::make_unique<iprocess::LoaderThread>(itemId, fullSize);

  thread->setAutoDelete(true);

  if (!hasMetrics(itemId))
  {
    thread->withMetrics(true);
    QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::metricsReady,
                     this, &Cache::metricsReadyFromThread, Qt::QueuedConnection);
  }

  QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::imageReady,
                   this, &Cache::imageReadyFromThread, Qt::QueuedConnection);

  return thread;
}

void Cache::startThreadForItem(pcontainer::ItemId const& itemId) const
{
  LOG(DEBUG) << "[Cache] Requested thread load for " << itemId.fileName;
  if (utils::valueIn(itemId.fileName, alreadyLoading))
  {
    LOG(DEBUG) << "[Cache] Already loading " << itemId.fileName;
    return;
  }

  alreadyLoading.insert(itemId.fileName);

  auto thread = makeLoadingThread(itemId);
  QThreadPool::globalInstance()->start(thread.release());
}

void Cache::imageReadyFromThread(pcontainer::ItemId itemId, QImage image)
{
  // TODO: BUG! Handle when image is NULL, as this can happen when in OOM conditions
  // Currently the code goes into infinite loop requesting new preload and failing to deliver one from cache
  // because preload is in fact null

  fullImageCache.replace(itemId.fileName, image);

  auto& preload = preloadImageCache[itemId.fileName];
  if (preload.isNull())
  {
      auto const preloadSize = config::qSize("imageCache.preloadSize", QSize(320, 240));
      preload = image.scaled(preloadSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
      LOG(DEBUG) << "[Cache] Saved new preload image " << itemId.fileName;
  }

  auto const uuidIt = alreadyLoading.find(itemId.fileName);
  assert(uuidIt != alreadyLoading.end());

  alreadyLoading.erase(uuidIt);

  emit updateImage(itemId, image);
}

void Cache::metricsReadyFromThread(pcontainer::ItemId itemId, iprocess::MetricPtr metrics)
{
  metricCache.emplace(itemId.fileName, metrics);
  LOG(DEBUG) << "[Cache] Saved new metrics for " << itemId.fileName;

  auto const& series = photoSet.findSeries(itemId.seriesUuid);

  if (!std::all_of(series->begin(), series->end(),
        [this](pcontainer::ItemPtr const& item){
            return utils::valueIn(item->fileName(), metricCache);
        }))
  {
    // not all metrics done -> emit just current one
    emit updateMetrics(itemId, metrics);
    return;
  }

  auto allMetrics = utils::transformToVector<iprocess::MetricPtr>(series->begin(), series->end(),
      [this](auto const& item){ return metricCache[item->fileName()]; });

  LOG(DEBUG) << "[Cache] Aggregating metrics for series " << itemId.seriesUuid.toString() << " after photo " << itemId.fileName;
  iprocess::aggregateMetrics(allMetrics);

  bool const doLog = config::qualified("logging.metrics", false);
  for (std::size_t i = 0; i < series->size(); ++i)
  {
    auto const& filename = series->item(i)->fileName();
    auto const& metric = metricCache[filename];
    emit updateMetrics(pcontainer::ItemId{itemId.seriesUuid, filename}, metric);

    LOG_IF(doLog, DEBUG) << "Calculated series metrics" << std::endl
       << "photoItem: " << filename << std::endl
       << "metric: " << metric;
  }
}

bool Cache::hasMetrics(pcontainer::ItemId const& itemId) const
{
    return utils::valueIn(itemId.fileName, metricCache);
}

iprocess::MetricPtr Cache::getMetrics(pcontainer::ItemId const& itemId) const
{
    auto const it = metricCache.find(itemId.fileName);
    if (it == metricCache.end())
      return nullptr;

    LOG(DEBUG) << "[Cache] Retrieved metrics for " << itemId.fileName;
    return it->second;
}

}} // namespace phobos::icache
