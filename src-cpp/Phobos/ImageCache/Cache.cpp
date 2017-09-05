#include "PhotoContainers/Series.h"
#include "ImageCache/Cache.h"
#include "Utils/Algorithm.h"
#include "Utils/Preload.h"
#include "Utils/Asserted.h"
#include "ConfigExtension.h"
#include "qt_ext/qexifimageheader.h"
#include <easylogging++.h>

// TODO: Maybe use transaction-like logic and log TRXID

namespace phobos { namespace icache {

Cache::Cache(pcontainer::Set const& photoSet) :
    photoSet(photoSet), metricCache(photoSet)
{
  QObject::connect(&metricCache, &MetricCache::updateMetrics, this, &Cache::updateMetrics);
}

class Cache::Transaction
{
public:
  struct Factory;
  struct Group;

  Transaction(Cache & cache, pcontainer::ItemId const& itemId);
  QImage operator()() const;

  QUuid const uuid;
  pcontainer::ItemId const itemId;

private:
  enum class Type { Full, Thumbnail };
  Transaction& setType(Type const type) & { this->type = type; return *this; }
  Transaction&& setType(Type const type) && { this->type = type; return std::move(*this); }

  void startThread() const;

  Cache & cache;
  Type type = Type::Full;
  bool mutable threadStarted = false;
};

Cache::Transaction::Transaction(Cache & cache, pcontainer::ItemId const& itemId) :
  uuid(QUuid::createUuid()), itemId(itemId), cache(cache)
{
}

namespace {
  QImage getInitialThumbnail(pcontainer::ItemId const& itemId)
  {
    QImage const exifThumb = QExifImageHeader(itemId.fileName).thumbnail();
    if (!exifThumb.isNull())
    {
      LOG(DEBUG) << "[Cache] Returned initial EXIF thumbnail for " << itemId.fileName;
      return exifThumb;
    }

    // TODO: use thumbnails from OS if available
    // https://stackoverflow.com/questions/19523599/how-to-get-thumbnail-of-file-using-the-windows-api
    static QImage const preloadImage =
      utils::preloadImage(config::qSize("imageCache.preloadSize", QSize(320, 240)));

    LOG(DEBUG) << "[Cache] Returned initial blank thumbnail for " << itemId.fileName;
    return preloadImage;
  }
} // unnamed namespace

QImage Cache::Transaction::operator()() const
{
  if (type == Type::Full)
  {
    QImage const fullImage = cache.fullImageCache.find(itemId.fileName);
    if (!fullImage.isNull())
    {
      LOG(DEBUG) << "[Cache] Returned full image for " << itemId.fileName;
      return fullImage;
    }

    startThread();
  }

  auto const thumbImageIt = cache.thumbnailCache.find(itemId.fileName);
  if (thumbImageIt != cache.thumbnailCache.end() && !thumbImageIt->second.isNull())
  {
    LOG(DEBUG) << "[Cache] Returned thumbnail for " << itemId.fileName;
    return thumbImageIt->second;
  }

  startThread();
  return getInitialThumbnail(itemId);
}

void Cache::Transaction::startThread() const
{
  if (threadStarted)
    return;

  cache.startThreadForItem(itemId);
  threadStarted = true;
}

struct Cache::Transaction::Group
{
  using TransactionVec = std::vector<Transaction>;
  using Result = std::map<pcontainer::ItemId, QImage>;

  Result operator()() const
  {
    Result result;
    for (auto const& t : transactions)
      result.emplace(t.itemId, t());
    return result;
  }

  Group& operator+=(Transaction && t)
  {
    transactions.emplace_back(std::move(t));
    return *this;
  }

private:
  TransactionVec transactions;
};

struct Cache::Transaction::Factory
{
public:
  static Transaction singlePhoto(Cache & cache, pcontainer::ItemId const& itemId)
  {
    return Transaction(cache, itemId);
  }

  static Transaction singleThumbnail(Cache & cache, pcontainer::ItemId const& itemId)
  {
    return Transaction(cache, itemId).setType(Transaction::Type::Thumbnail);
  }

  static Transaction::Group seriesPhotos(Cache & cache, QUuid const& seriesId)
  {
    Transaction::Group result;
    for (pcontainer::ItemPtr const& photo : utils::asserted::fromPtr(cache.photoSet.findSeries(seriesId)))
      result += Transaction(cache, photo->id());
    return result;
  }

  static Transaction::Group seriesThumbnails(Cache & cache, QUuid const& seriesId)
  {
    Transaction::Group result;
    for (pcontainer::ItemPtr const& photo : utils::asserted::fromPtr(cache.photoSet.findSeries(seriesId)))
      result += Transaction(cache, photo->id()).setType(Transaction::Type::Thumbnail);
    return result;
  }
};

QImage Cache::getImage(pcontainer::ItemId const& itemId)
{
  LOG(DEBUG) << "[Cache] Requested full image for " << itemId.fileName;
  return Transaction::Factory::singlePhoto(*this, itemId)();
}

QImage Cache::getThumbnail(pcontainer::ItemId const& itemId)
{
  LOG(DEBUG) << "[Cache] Requested thumbnail for " << itemId.fileName;
  return Transaction::Factory::singleThumbnail(*this, itemId)();
}

std::map<pcontainer::ItemId, QImage> Cache::getImages(QUuid const& seriesId)
{
  LOG(DEBUG) << "[Cache] Requested full images for series " << seriesId.toString();
  return Transaction::Factory::seriesPhotos(*this, seriesId)();
}

std::map<pcontainer::ItemId, QImage> Cache::getThumbnails(QUuid const& seriesId)
{
  LOG(DEBUG) << "[Cache] Requested thumbnails for series " << seriesId.toString();
  return Transaction::Factory::seriesThumbnails(*this, seriesId)();
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

void Cache::startThreadForItem(pcontainer::ItemId const& itemId)
{
  LOG(DEBUG) << "[Cache] Requested thread load for " << itemId.fileName;
  if (utils::valueIn(itemId.fileName, alreadyLoading))
  {
    LOG(DEBUG) << "[Cache] Already loading " << itemId.fileName;
    return;
  }

  alreadyLoading.insert(itemId.fileName);

  auto thread = makeLoadingThread(itemId);
  threadPool.start(std::move(thread), 0);
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

  auto const uuidIt = alreadyLoading.find(itemId.fileName);
  assert(uuidIt != alreadyLoading.end());

  alreadyLoading.erase(uuidIt);

  emit updateImage(itemId, image);
}

}} // namespace phobos::icache
