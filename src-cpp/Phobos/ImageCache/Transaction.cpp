#include "ImageCache/Transaction.h"
#include "ImageCache/Cache.h"
#include "ConfigExtension.h"
#include "Utils/Preload.h"
#include "Utils/Asserted.h"
#include "qt_ext/qexifimageheader.h"
#include <easylogging++.h>

namespace phobos { namespace icache {

Transaction::Transaction(Cache& cache) :
  uuid(QUuid::createUuid()), cache(cache)
{
}

Transaction&& Transaction::callback(CallbackType && newCallback) &&
{
  loadCallback = OptCallback{std::move(newCallback)};
  return std::move(*this);
}

void Transaction::OptCallback::operator()(Result && result) const
{
  if (func)
    func(std::move(result));
}

void Transaction::OptCallback::operator()(QImage const& im, ImageQuality qual) const
{
  if (func)
    func(Result{im, qual});
}

QString Transaction::toString() const
{
  return QString("[CacheTransaction]: Get %1 %2 %3")
    .arg(onlyThumbnail ? "thumbnail" : "full image")
    .arg(itemId.fileName)
    .arg(disableLoading ? "from cache" : "with loading");
}

namespace {
  Transaction::Result getInitialThumbnail(pcontainer::ItemId const& itemId)
  {
    QImage const exifThumb = QExifImageHeader(itemId.fileName).thumbnail();
    if (!exifThumb.isNull())
    {
      LOG(DEBUG) << "[Cache] Returned initial EXIF thumbnail for " << itemId.fileName;
      return {exifThumb, Transaction::ImageQuality::ExifThumb};
    }

    // TODO: use thumbnails from OS if available
    // https://stackoverflow.com/questions/19523599/how-to-get-thumbnail-of-file-using-the-windows-api
    static QImage const preloadImage =
      utils::preloadImage(config::qSize("imageCache.preloadSize", QSize(320, 240)));

    LOG(DEBUG) << "[Cache] Returned initial blank thumbnail for " << itemId.fileName;
    return {preloadImage, Transaction::ImageQuality::Blank};
  }
} // unnamed namespace

Transaction::Result Transaction::execute() &&
{
  return cache.execute(std::move(*this));
}

Transaction::Result Transaction::operator()() const
{
  if (!itemId)
  {
    LOG(ERROR) << "[Cache] Invalid transaction";
    return {QImage{}, ImageQuality::None};
  }

  if (!onlyThumbnail)
  {
    QImage const fullImage = cache.fullImageCache.find(itemId.fileName);
    if (!fullImage.isNull())
    {
      LOG(DEBUG) << "[Cache] Returned full image for " << itemId.fileName;
      return {fullImage, ImageQuality::Full};
    }

    _shouldStartThread = true;
  }

  auto const thumbImageIt = cache.thumbnailCache.find(itemId.fileName);
  if (thumbImageIt != cache.thumbnailCache.end() && !thumbImageIt->second.isNull())
  {
    LOG(DEBUG) << "[Cache] Returned thumbnail for " << itemId.fileName;
    return {thumbImageIt->second, ImageQuality::Thumb};
  }

  _shouldStartThread = true;
  return getInitialThumbnail(itemId);
}

auto TransactionGroup::operator()() const -> Result
{
  Result result;
  for (auto const& t : transactions)
    result.emplace(t.getItemId(), t());
  return result;
}

TransactionGroup& TransactionGroup::operator+=(Transaction && t)
{
  transactions.emplace_back(std::move(t));
  return *this;
}

TransactionGroup& TransactionGroup::operator+=(Transaction const& t)
{
  transactions.emplace_back(t);
  return *this;
}

}} // namespace phobos::icache
