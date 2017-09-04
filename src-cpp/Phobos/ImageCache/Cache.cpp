#include "PhotoContainers/Series.h"
#include "ImageCache/Cache.h"
#include "Utils/Algorithm.h"
#include "Utils/Preload.h"
#include "Utils/Asserted.h"
#include "ConfigExtension.h"
#include "ImageProcessing/MetricsAggregate.h"
#include "ImageProcessing/MetricsIO.h"
#include "qt_ext/qexifimageheader.h"
#include <easylogging++.h>

// TODO: Maybe use transaction-like logic and log TRXID

namespace phobos { namespace icache {

Cache::Cache(pcontainer::Set const& photoSet) :
    photoSet(photoSet)
{
  QObject::connect(&photoSet, &pcontainer::Set::changedSeries, this, &Cache::updateSeriesMetrics);
}

class Cache::Transaction
{
public:
  struct Factory;

  Transaction(Cache & cache, pcontainer::ItemId const& itemId);
  QImage operator()();

  QUuid const uuid;

private:
  enum class Type { Full, Thumbnail };
  void setType(Type const type) { this->type = type; }
  void startThread();

  Cache & cache;
  pcontainer::ItemId const itemId;

  Type type = Type::Full;
  bool threadStarted = false;
};

Cache::Transaction::Transaction(Cache & cache, pcontainer::ItemId const& itemId) :
  uuid(QUuid::createUuid()), cache(cache), itemId(itemId)
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

QImage Cache::Transaction::operator()()
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

void Cache::Transaction::startThread()
{
  if (threadStarted)
    return;

  cache.startThreadForItem(itemId);
  threadStarted = true;
}

struct Cache::Transaction::Factory
{
public:
  static Transaction singlePhoto(Cache & cache, pcontainer::ItemId const& itemId)
  {
    LOG(DEBUG) << "[Cache] Requested full image for " << itemId.fileName;
    Transaction result(cache, itemId);
    return result;
  }

  static Transaction singleThumbnail(Cache & cache, pcontainer::ItemId const& itemId)
  {
    LOG(DEBUG) << "[Cache] Requested thumbnail for " << itemId.fileName;
    Transaction result(cache, itemId);
    result.setType(Transaction::Type::Thumbnail);
    return result;
  }
};

std::map<pcontainer::ItemId, QImage> Cache::getImages(QUuid const& seriesId)
{
  LOG(DEBUG) << "[Cache] Requested full images for series " << seriesId.toString().toStdString();

  pcontainer::SeriesPtr const series = photoSet.findSeries(seriesId);
  assert(series);

  std::map<pcontainer::ItemId, QImage> result;

  for (pcontainer::ItemPtr const& photo : *series)
  {
    QImage const fullImage = fullImageCache.find(photo->id().fileName);
    if (!fullImage.isNull())
    {
      LOG(DEBUG) << "[Cache] Returned full image for " << photo->id().fileName;
      result.emplace(photo->id(), fullImage);
    }
    else
    {
      startThreadForItem(photo->id());
      result.emplace(photo->id(), getThumbnailWithLoading(photo->id(), false));
    }
  }

  return result;
}

std::map<pcontainer::ItemId, QImage> Cache::getThumbnails(QUuid const& seriesId)
{
  LOG(DEBUG) << "[Cache] Requested thumbnails for series " << seriesId.toString().toStdString();

  pcontainer::SeriesPtr const series = photoSet.findSeries(seriesId);
  assert(series);

  std::map<pcontainer::ItemId, QImage> result;

  for (pcontainer::ItemPtr const& photo : *series)
    result.emplace(photo->id(), getThumbnailWithLoading(photo->id(), true));

  return result;
}

QImage Cache::getImage(pcontainer::ItemId const& itemId)
{
  return Transaction::Factory::singlePhoto(*this, itemId)();
}

QImage Cache::getThumbnail(pcontainer::ItemId const& itemId)
{
  return Transaction::Factory::singleThumbnail(*this, itemId)();
}

QImage Cache::getThumbnailWithLoading(pcontainer::ItemId const& itemId, bool requestLoad)
{
  auto it = thumbnailCache.find(itemId.fileName);
  if (it != thumbnailCache.end() && !it->second.isNull())
  {
    LOG(DEBUG) << "[Cache] Returned thumbnail for " << itemId.fileName;
    return it->second;
  }

  if (requestLoad)
    startThreadForItem(itemId);

  return getInitialThumbnail(itemId);
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

void Cache::metricsReadyFromThread(pcontainer::ItemId itemId, iprocess::MetricPtr metrics)
{
  metricCache.emplace(itemId.fileName, metrics);
  LOG(DEBUG) << "[Cache] Saved new metrics for " << itemId.fileName;

  if (!updateSeriesMetrics(itemId.seriesUuid))
    emit updateMetrics(itemId, metrics); // if not all metrics are loaded, emit just for the current one
}

bool Cache::updateSeriesMetrics(QUuid const& seriesUuid)
{
  auto const& series = photoSet.findSeries(seriesUuid);

  if (!std::all_of(series->begin(), series->end(),
        [this](pcontainer::ItemPtr const& item){
            return utils::valueIn(item->fileName(), metricCache);
        }))
  {
    return false;
  }

  auto allMetrics = utils::transformToVector<iprocess::MetricPtr>(series->begin(), series->end(),
      [this](auto const& item){ return metricCache[item->fileName()]; });

  LOG(DEBUG) << "[Cache] Aggregating metrics for series " << seriesUuid.toString();
  iprocess::aggregateMetrics(allMetrics);

  bool const doLog = config::qualified("logging.metrics", false);
  for (std::size_t i = 0; i < series->size(); ++i)
  {
    auto const& filename = series->item(i)->fileName();
    auto const& metric = metricCache[filename];
    emit updateMetrics(pcontainer::ItemId{seriesUuid, filename}, metric);

    LOG_IF(doLog, DEBUG) << "Calculated series metrics" << std::endl
       << "photoItem: " << filename << std::endl
       << "metric: " << metric;
  }

  return true;
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
