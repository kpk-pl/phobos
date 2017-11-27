#include "ImageCache/LoadingManager.h"
#include "ImageCache/Cache.h"
#include "Config.h"
#include "ConfigExtension.h"
#include <easylogging++.h>

namespace phobos { namespace icache {

LoadingManager::LoadingManager(Cache const& cache) : cache(cache)
{
}

void LoadingManager::start(Transaction && job)
{
  pcontainer::ItemId const itemId = job.getItemId();

  LOG(DEBUG) << "[Cache] Requested thread load for " << itemId.fileName;
  auto thread = makeLoadingThread(itemId);
  transactionsInThread.emplace(itemId, std::make_pair(thread->uuid(), std::move(job)));
  threadPool.start(std::move(thread), 0);

// TODO: use generations to start as priorities. Handle persistent flag from transaction.
// Don't start loading proactively when there is no more cache space left for given generation
}

void LoadingManager::stop(pcontainer::ItemId const& itemId)
{
  auto const allTrans = transactionsInThread.equal_range(itemId);
  for (auto it = allTrans.first; it != allTrans.second; ++it)
    threadPool.cancel(it->second.first);
  transactionsInThread.erase(allTrans.first, allTrans.second);
}

std::unique_ptr<iprocess::LoaderThread> LoadingManager::makeLoadingThread(pcontainer::ItemId const& itemId) const
{
  QSize const fullSize = config::qSize("imageCache.fullSize", QSize(1920, 1080));
  auto thread = std::make_unique<iprocess::LoaderThread>(itemId, fullSize);

  thread->setAutoDelete(true);

  if (!cache.metrics().has(itemId))
  {
    thread->withMetrics(true);
    QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::metricsReady,
                     this, &LoadingManager::metricsReady, Qt::QueuedConnection);
  }

  QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::imageReady,
                   this, &LoadingManager::imageLoaded, Qt::QueuedConnection);

  return thread;
}

void LoadingManager::imageLoaded(pcontainer::ItemId const& itemId, QImage const& image)
{
  auto const allTrans = transactionsInThread.equal_range(itemId);
  if (allTrans.first == allTrans.second)
    return;

  QImage thumbnail;

  auto const thumbIt = cache.thumbnails().find(itemId.fileName);
  if (thumbIt == cache.thumbnails().end())
  {
    auto const preloadSize = config::qSize("imageCache.preloadSize", QSize(320, 240));
    thumbnail = image.scaled(preloadSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    emit thumbnailReady(itemId, thumbnail);
  }
  else
  {
    thumbnail = thumbIt->second;
  }

  bool updateFullCache = false;

  for (auto it = allTrans.first; it != allTrans.second; ++it)
  {
    if (it->second.second.isThumbnail())
      it->second.second.getCallback()(Result{thumbnail, ImageQuality::Thumb, true});
    else
    {
      updateFullCache = true;
      it->second.second.getCallback()(Result{image, ImageQuality::Full, true});
    }
  }

  if (updateFullCache)
    emit imageReady(itemId, image);

  transactionsInThread.erase(allTrans.first, allTrans.second);
}

}} // namespace phobos::icache
