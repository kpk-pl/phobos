#include "PhotoContainers/Series.h"
#include "Utils/Preload.h"
#include "ImageCache/Cache.h"
#include "Utils/Algorithm.h"
#include "Utils/Asserted.h"
#include "ConfigExtension.h"
#include <easylogging++.h>
#include <algorithm>
#include <QPixmapCache>

namespace phobos { namespace icache {

Cache::Cache(pcontainer::Set const& photoSet) :
  metricCache(photoSet), photoSet(photoSet), loadingManager(*this), scheduler(photoSet)
{
  QObject::connect(&metricCache, &MetricCache::updateMetrics, this, &Cache::updateMetrics);
  QObject::connect(&photoSet, &pcontainer::Set::changedSeries, this, &Cache::changedSeries);
  QObject::connect(&loadingManager, &LoadingManager::imageReady, this, &Cache::imageReady);
  QObject::connect(&loadingManager, &LoadingManager::thumbnailReady, this, &Cache::thumbnailReady);
  QObject::connect(&loadingManager, &LoadingManager::metricsReady, &metricCache, &MetricCache::newLoadedFromThread);
}

Result Cache::execute(ConstTransactionPtr transaction)
{
  LOG(DEBUG) << transaction->toString();

  auto const& photoItem = photoSet.findItem(transaction->itemId);
  if (!photoItem)
    return {QImage{}, ImageQuality::None, false};

  auto const result = executeImpl(*transaction);
  scheduleTransaction(std::move(transaction));
  return result;
}

void Cache::scheduleTransaction(ConstTransactionPtr && transaction)
{
  ConstTransactionPtrVec schedule = scheduler(std::move(transaction));

  auto const part = std::stable_partition(schedule.begin(), schedule.end(), [this](ConstTransactionPtr const& trans){
    if (trans->imageSize == ImageSize::Thumbnail)
      return thumbnailCache.find(trans->itemId.fileName) == thumbnailCache.end();
    return !fullImageCache.has(trans->itemId.fileName);
  });

  std::for_each(part, schedule.end(), [this](ConstTransactionPtr const& trans){
    if (trans->imageSize == ImageSize::Full)
      fullImageCache.touch(trans->itemId.fileName, trans->priority);
  });
  schedule.erase(part, schedule.end());

  loadingManager.start(std::move(schedule));
}

namespace {
QImage trimExifThumbnail(QImage const thumb, QSize const size)
{
  // try not to mess with thumb image because scaling it will create a Copy On Write

  auto const expectedSize = size.scaled(thumb.size(), Qt::KeepAspectRatio);
  if (expectedSize == size)
    return thumb;

  /*
   * Some cameras save thumbnails with added margins. Those margins are not in uniform color and they
   * change image aspect ratio.
   */
  QRect const window(QPoint((thumb.width() - expectedSize.width() + 1)/2, // rounding up
                            (thumb.height() - expectedSize.height() + 1)/2),
                     expectedSize);

  return thumb.copy(window);
}

Result getInitialThumbnail(pcontainer::Item const& item)
{
  if (!item.info().thumbnail.isNull())
  {
    LOG(DEBUG) << "[Cache] Returned initial EXIF thumbnail for " << item.id().fileName;
    return {trimExifThumbnail(item.info().thumbnail, item.info().size), ImageQuality::ExifThumb, false};
  }

  // TODO: use thumbnails from OS if available
  // https://stackoverflow.com/questions/19523599/how-to-get-thumbnail-of-file-using-the-windows-api

  // TODO: use imageCache for different preload Size -> most of them will be used with the same size
  // when images are from one single camera

  QSize const preloadSize =
      item.info().size.scaled(config::qSize("imageCache.preloadSize", QSize(320, 240)),
                              Qt::KeepAspectRatio);

  LOG(DEBUG) << "[Cache] Returned initial blank thumbnail for " << item.id().fileName;
  return {utils::preloadImage(preloadSize), ImageQuality::Blank, false};
}
} // unnamed namespace

Result Cache::executeImpl(Transaction const& transaction) const
{
  auto const& itemId = transaction.itemId;
  if (!itemId)
  {
    LOG(ERROR) << "[Cache] Invalid transaction";
    return {QImage{}, ImageQuality::None, false};
  }

  bool sufficient = true;
  if (transaction.imageSize == ImageSize::Full)
  {
    QImage const fullImage = fullImageCache.find(itemId.fileName);
    if (!fullImage.isNull())
    {
      LOG(DEBUG) << "[Cache] Returned full image for " << itemId.fileName;
      return {fullImage, ImageQuality::Full, true};
    }

    sufficient = false;
  }

  auto const thumbImageIt = thumbnailCache.find(itemId.fileName);
  if (thumbImageIt != thumbnailCache.end() && !thumbImageIt->second.isNull())
  {
    LOG(DEBUG) << "[Cache] Returned thumbnail for " << itemId.fileName;
    return {thumbImageIt->second, ImageQuality::Thumb, sufficient};
  }

  auto const item = photoSet.findItem(itemId);
  if (!item)
  {
    LOG(ERROR) << "[Cache] Cannot locate transaction item " << itemId << " in photo set";
    return {QImage{}, ImageQuality::None, false};
  }

  return getInitialThumbnail(*item);
}

void Cache::changedSeries(QUuid const& seriesUuid)
{
  auto const& series = photoSet.findSeries(seriesUuid);
  for (auto const& itemId : series.removedItems())
  {
    loadingManager.stop(itemId);
    thumbnailCache.erase(itemId.fileName);
    fullImageCache.erase(itemId.fileName);
  }
}

void Cache::thumbnailReady(pcontainer::ItemId const& itemId, QImage const& image)
{
  thumbnailCache[itemId.fileName] = image;
  LOG(DEBUG) << "[Cache] Saved new preload image " << itemId.fileName;
}

void Cache::imageReady(pcontainer::ItemId const& itemId, QImage const& image, Priority const& priority)
{
  fullImageCache.replace(itemId.fileName, image, priority);
}

}} // namespace phobos::icache
