#include "ImageCache/Transaction.h"
#include "ImageCache/Cache.h"

namespace phobos { namespace icache {

Transaction::Transaction(Cache& cache) :
  uuid(QUuid::createUuid()), cache(cache)
{}

Transaction&& Transaction::callback(TransactionCallback && newCallback) &&
{
  loadCallback = OptTransactionCallback{std::move(newCallback)};
  return std::move(*this);
}

QString Transaction::toString() const
{
  QString s = QString("[CacheTransaction]: Get %1 %2 %3")
    .arg(imageSize == ImageSize::Thumbnail ? "thumbnail" : "full image")
    .arg(itemId.fileName)
    .arg(loadingMode == LoadingMode::Cached ? "from cache" : "with loading");

  if (predictionMode == PredictionMode::Proactive)
    s += " proactively";

  if (persistency == Persistency::Yes)
    s += " persistently";

  return s;
}

Result Transaction::execute() &&
{
  return cache.execute(std::make_shared<Transaction>(std::move(*this)));
}

TransactionPtr Transaction::cloneFor(pcontainer::ItemId const& id) const
{
  TransactionPtr clone = std::make_shared<Transaction>(cache);
  clone->itemId = id;
  clone->imageSize = imageSize;
  clone->loadingMode = loadingMode;
  clone->predictionMode = predictionMode;
  clone->persistency = persistency;
  clone->loadCallback = {nullptr};
  return clone;
}

}} // namespace phobos::icache
