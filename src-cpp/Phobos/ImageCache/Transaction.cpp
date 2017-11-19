#include "ImageCache/Transaction.h"
#include "ImageCache/Cache.h"

namespace phobos { namespace icache {

Transaction::Transaction(Cache& cache) :
  uuid(QUuid::createUuid()), cache(cache)
{}

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
  QString s = QString("[CacheTransaction]: Get %1 %2 %3")
    .arg(onlyThumbnail ? "thumbnail" : "full image")
    .arg(itemId.fileName)
    .arg(disableLoading ? "from cache" : "with loading");

  if (proactiveLoading)
    s += " proactively";

  if (persistentLoading)
    s += " persistently";

  return s;
}

Result Transaction::execute() &&
{
  return cache.execute(std::move(*this));
}

}} // namespace phobos::icache
