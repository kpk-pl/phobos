#ifndef IMAGECACHE_TRANSACTION_H_
#define IMAGECACHE_TRANSACTION_H_

#include "ImageCache/CacheFwd.h"
#include "ImageCache/TransactionFwd.h"
#include "PhotoContainers/ItemId.h"
#include <QImage>
#include <QUuid>
#include <functional>

namespace phobos { namespace icache {

class Transaction
{
public:
  Transaction(Cache& cache);

  Transaction&& item(pcontainer::ItemId const& itemId) && { this->itemId = itemId; return std::move(*this); }
  Transaction&& thumbnail() && { onlyThumbnail = true; return std::move(*this); }
  Transaction&& onlyCache() && { disableLoading = true; return std::move(*this); }
  Transaction&& callback(TransactionCallback && newCallback) &&;
  Transaction&& proactive() && { proactiveLoading = true; return std::move(*this); }
  Transaction&& persistent() && { persistentLoading = true; return std::move(*this); }

  Result execute() &&;

  QString toString() const;

  QUuid const uuid;
  pcontainer::ItemId const& getItemId() const { return itemId; }
  OptTransactionCallback const& getCallback() const { return loadCallback; }
  bool isThumbnail() const { return onlyThumbnail; }
  bool loadingEnabled() const { return !disableLoading; }

private:
  Cache& cache;

  pcontainer::ItemId itemId;
  bool onlyThumbnail = false;
  bool disableLoading = false;
  bool proactiveLoading = false;
  bool persistentLoading = false;
  OptTransactionCallback loadCallback;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_TRANSACTION_H_
