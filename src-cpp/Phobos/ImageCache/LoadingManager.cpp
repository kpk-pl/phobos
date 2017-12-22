#include "ImageCache/LoadingManager.h"
#include "ImageCache/Cache.h"
#include "Config.h"
#include "ConfigExtension.h"
#include <easylogging++.h>

namespace phobos { namespace icache {

LoadingManager::LoadingManager(Cache const& cache) : cache(cache)
{}

void LoadingManager::start(LoadingJobVec && jobs)
{
  for (auto job : jobs)
    startOne(std::move(job));
}

void LoadingManager::startOne(LoadingJob && job)
{
  pcontainer::ItemId const itemId = job.itemId;

  LOG(DEBUG) << "[Cache] Requested thread load for " << itemId.fileName;
  auto thread = makeLoadingThread(itemId);
  jobsInThread.emplace(itemId, std::make_pair(thread->uuid(), std::move(job)));
  threadPool.start(std::move(thread), job.generation);
}

void LoadingManager::stop(pcontainer::ItemId const& itemId)
{
  auto const allTrans = jobsInThread.equal_range(itemId);
  for (auto it = allTrans.first; it != allTrans.second; ++it)
    threadPool.cancel(it->second.first);
  jobsInThread.erase(allTrans.first, allTrans.second);
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
  auto const allTrans = jobsInThread.equal_range(itemId);
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
  Generation biggestGeneration = 0;

  for (auto it = allTrans.first; it != allTrans.second; ++it)
  {
    if (it->second.second.onlyThumbnail)
      it->second.second.callback(Result{thumbnail, ImageQuality::Thumb, true});
    else
    {
      updateFullCache = true;
      biggestGeneration = std::max(biggestGeneration, it->second.second.generation);
      it->second.second.callback(Result{image, ImageQuality::Full, true});
    }
  }

  if (updateFullCache)
    emit imageReady(itemId, image, biggestGeneration);

  jobsInThread.erase(allTrans.first, allTrans.second);
}

}} // namespace phobos::icache
