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
}

QImage Cache::getImage(pcontainer::ItemId const& itemId)
{
  LOG(DEBUG) << "[Cache] Requested full image for " << itemId.fileName;
  return execute(Transaction::Factory::singlePhoto(*this, itemId));
}

QImage Cache::getThumbnail(pcontainer::ItemId const& itemId)
{
  LOG(DEBUG) << "[Cache] Requested thumbnail for " << itemId.fileName;
  return execute(Transaction::Factory::singleThumbnail(*this, itemId));
}

std::map<pcontainer::ItemId, QImage> Cache::getImages(QUuid const& seriesId)
{
  LOG(DEBUG) << "[Cache] Requested full images for series " << seriesId.toString();
  return execute(Transaction::Factory::seriesPhotos(*this, seriesId));
}

std::map<pcontainer::ItemId, QImage> Cache::getThumbnails(QUuid const& seriesId)
{
  LOG(DEBUG) << "[Cache] Requested thumbnails for series " << seriesId.toString();
  return execute(Transaction::Factory::seriesThumbnails(*this, seriesId));
}

QImage Cache::execute(Transaction && transaction)
{
  QImage const result = transaction();
  if (transaction.startThread())
    startThreadForItem(std::move(transaction));

  return result;
}

std::map<pcontainer::ItemId, QImage> Cache::execute(TransactionGroup && group)
{
  std::map<pcontainer::ItemId, QImage> result;

  for (auto & tran : group.transactions)
  {
    pcontainer::ItemId const id = tran.itemId;
    result.emplace(id, execute(std::move(tran)));
  }

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
  pcontainer::ItemId const itemId = transaction.itemId;

  LOG(DEBUG) << "[Cache] Requested thread load for " << itemId.fileName;
  transactionsInThread.emplace(itemId, std::move(transaction));
  threadPool.start(makeLoadingThread(itemId), 0);

  // TODO: prioritize loading of full images
}

void Cache::imageReadyFromThread(pcontainer::ItemId itemId, QImage image)
{
  // TODO: BUG! Handle when image is NULL, as this can happen when in OOM conditions
  // Currently the code goes into infinite loop requesting new preload and failing to deliver one from cache
  // because preload is in fact null

  fullImageCache.replace(itemId.fileName, image);

  auto& preload = thumbnailCache[itemId.fileName];
  if (preload.isNull())
  {
    auto const preloadSize = config::qSize("imageCache.preloadSize", QSize(320, 240));
    preload = image.scaled(preloadSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    LOG(DEBUG) << "[Cache] Saved new preload image " << itemId.fileName;
  }

  emit updateImage(itemId, image);

  auto const allTrans = transactionsInThread.equal_range(itemId);
  for (auto it = allTrans.first; it != allTrans.second; ++it)
    ; // do transaction callbacks

  transactionsInThread.erase(allTrans.first, allTrans.second);
}

}} // namespace phobos::icache
