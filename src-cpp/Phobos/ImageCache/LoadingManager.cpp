#include "ImageCache/LoadingManager.h"
#include "ImageCache/Cache.h"
#include "Config.h"
#include "ConfigExtension.h"
#include <easylogging++.h>

namespace phobos { namespace icache {

LoadingManager::LoadingManager(Cache const& cache) : cache(cache)
{}

void LoadingManager::start(ConstTransactionPtrVec && schedule)
{
  for (auto & job : schedule)
    startOne(std::move(job));
}

// TODO: Consider creating a separate thread for reading images from disk
// That would do this serially, communicate over promise-future with processing
// thread.
// I believe that would serialize processing and maybe speed up reading from disk a bit
// Probably it would be then a good idea to adjust number of processing threads
void LoadingManager::startOne(ConstTransactionPtr && transaction)
{
  pcontainer::ItemId const itemId = transaction->itemId;

  LOG(DEBUG) << "[Cache] Requested thread load for " << itemId.fileName;

  auto thread = makeLoadingThread(itemId);
  jobsInThread.emplace(itemId, std::make_pair(thread->uuid(), transaction));
  threadPool.start(std::move(thread), transaction);
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

  boost::optional<Priority> biggestPriority;

  for (auto it = allTrans.first; it != allTrans.second; ++it)
  {
    ConstTransactionPtr const& tran = it->second.second;
    if (tran->imageSize == ImageSize::Thumbnail)
    {
      tran->loadCallback(Result{thumbnail, ImageQuality::Thumb, true});
    }
    else
    {
      if (biggestPriority < tran->priority)
        biggestPriority = tran->priority;
      tran->loadCallback(Result{image, ImageQuality::Full, true});
    }
  }

  if (biggestPriority)
    emit imageReady(itemId, image, *biggestPriority);

  jobsInThread.erase(allTrans.first, allTrans.second);
}

}} // namespace phobos::icache
