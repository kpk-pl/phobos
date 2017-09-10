#include "ImageCache/Transaction.h"
#include "ImageCache/Cache.h"
#include "ConfigExtension.h"
#include "Utils/Preload.h"
#include "Utils/Asserted.h"
#include "qt_ext/qexifimageheader.h"
#include <easylogging++.h>

namespace phobos { namespace icache {

Transaction Transaction::Factory::singlePhoto(Cache & cache, pcontainer::ItemId const& itemId)
{
  return Transaction(cache, itemId);
}

Transaction Transaction::Factory::singleThumbnail(Cache & cache, pcontainer::ItemId const& itemId)
{
  return Transaction(cache, itemId).setType(Transaction::Type::Thumbnail);
}

TransactionGroup Transaction::Factory::seriesPhotos(Cache & cache, QUuid const& seriesId)
{
  TransactionGroup result;
  for (pcontainer::ItemPtr const& photo : utils::asserted::fromPtr(cache.photoSet.findSeries(seriesId)))
    result += Transaction(cache, photo->id());
  return result;
}

TransactionGroup Transaction::Factory::seriesThumbnails(Cache & cache, QUuid const& seriesId)
{
  TransactionGroup result;
  for (pcontainer::ItemPtr const& photo : utils::asserted::fromPtr(cache.photoSet.findSeries(seriesId)))
    result += Transaction(cache, photo->id()).setType(Transaction::Type::Thumbnail);
  return result;
}

Transaction::Transaction(Cache const& cache, pcontainer::ItemId const& itemId) :
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

QImage Transaction::operator()() const
{
  if (type == Type::Full)
  {
    QImage const fullImage = cache.fullImageCache.find(itemId.fileName);
    if (!fullImage.isNull())
    {
      LOG(DEBUG) << "[Cache] Returned full image for " << itemId.fileName;
      return fullImage;
    }

    shouldStartThread = true;
  }

  auto const thumbImageIt = cache.thumbnailCache.find(itemId.fileName);
  if (thumbImageIt != cache.thumbnailCache.end() && !thumbImageIt->second.isNull())
  {
    LOG(DEBUG) << "[Cache] Returned thumbnail for " << itemId.fileName;
    return thumbImageIt->second;
  }

  shouldStartThread = true;
  return getInitialThumbnail(itemId);
}

auto TransactionGroup::operator()() const -> Result
{
  Result result;
  for (auto const& t : transactions)
    result.emplace(t.itemId, t());
  return result;
}

TransactionGroup& TransactionGroup::operator+=(Transaction && t)
{
  transactions.emplace_back(std::move(t));
  return *this;
}

}} // namespace phobos::icache
