#include "PhotoContainers/Series.h"
#include "ImageCache/Cache.h"
#include "Utils/Algorithm.h"
#include "Utils/Asserted.h"
#include "ConfigExtension.h"
#include <easylogging++.h>

namespace phobos { namespace icache {

Cache::Cache(pcontainer::Set const& photoSet) :
    metricCache(photoSet), photoSet(photoSet)
{
  QObject::connect(&metricCache, &MetricCache::updateMetrics, this, &Cache::updateMetrics);
  QObject::connect(&photoSet, &pcontainer::Set::changedSeries, this, &Cache::changedSeries);
}

Transaction::Result Cache::execute(Transaction && transaction)
{
  LOG(DEBUG) << transaction.toString();
  auto const result = transaction();
  if (transaction.shouldStartThread())
    startThreadForItem(std::move(transaction));

  return result;
}

std::unique_ptr<iprocess::LoaderThread> Cache::makeLoadingThread(pcontainer::ItemId const& itemId) const
{
  QSize const fullSize = config::qSize("imageCache.fullSize", QSize(1920, 1080));
  auto thread = std::make_unique<iprocess::LoaderThread>(itemId, fullSize);

  thread->setAutoDelete(true);

  if (!metrics().has(itemId))
  {
    thread->withMetrics(true);
    QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::metricsReady,
                     &metricCache, &MetricCache::newLoadedFromThread, Qt::QueuedConnection);
  }

  QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::imageReady,
                   this, &Cache::imageReadyFromThread, Qt::QueuedConnection);

  return thread;
}

void Cache::startThreadForItem(Transaction && transaction)
{
  pcontainer::ItemId const itemId = transaction.getItemId();

  LOG(DEBUG) << "[Cache] Requested thread load for " << itemId.fileName;
  auto thread = makeLoadingThread(itemId);
  transactionsInThread.emplace(itemId, std::make_pair(thread->uuid(), std::move(transaction)));
  threadPool.start(std::move(thread), 0);

  // TODO: prioritize loading of full images
}

void Cache::changedSeries(QUuid const& seriesUuid)
{
  auto const& series = photoSet.findSeries(seriesUuid);
  for (auto const& itemId : series.removedItems())
  {
    auto const allTrans = transactionsInThread.equal_range(itemId);
    for (auto it = allTrans.first; it != allTrans.second; ++it)
      threadPool.cancel(it->second.first);
    transactionsInThread.erase(allTrans.first, allTrans.second);

    thumbnailCache.erase(itemId.fileName);
    fullImageCache.erase(itemId.fileName);
  }
}

void Cache::imageReadyFromThread(pcontainer::ItemId itemId, QImage image)
{
  auto const allTrans = transactionsInThread.equal_range(itemId);
  if (allTrans.first == allTrans.second)
    return;

  auto& thumb = thumbnailCache[itemId.fileName];
  if (thumb.isNull())
  {
    auto const preloadSize = config::qSize("imageCache.preloadSize", QSize(320, 240));
    thumb = image.scaled(preloadSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    LOG(DEBUG) << "[Cache] Saved new preload image " << itemId.fileName;
  }

  bool updateFullCache = false;

  for (auto it = allTrans.first; it != allTrans.second; ++it)
  {
    if (it->second.second.isThumbnail())
      it->second.second.getCallback()(thumb, Transaction::ImageQuality::Thumb);
    else
    {
      updateFullCache = true;
      it->second.second.getCallback()(image, Transaction::ImageQuality::Full);
    }
  }

  if (updateFullCache)
    fullImageCache.replace(itemId.fileName, image);

  transactionsInThread.erase(allTrans.first, allTrans.second);
}

}} // namespace phobos::icache
