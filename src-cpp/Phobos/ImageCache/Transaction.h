#ifndef IMAGECACHE_TRANSACTION_H_
#define IMAGECACHE_TRANSACTION_H_

#include "ImageCache/CacheFwd.h"
#include "ImageCache/Types.h"
#include "ImageCache/TransactionFwd.h"
#include "ImageCache/Priority.h"
#include "PhotoContainers/ItemId.h"
#include <QImage>
#include <QUuid>
#include <functional>

namespace phobos { namespace icache {

class Transaction
{
public:
  Transaction(Cache& cache);

  Transaction&& item(pcontainer::ItemId const& pItemId) && { itemId = pItemId; return std::move(*this); }
  Transaction&& thumbnail() && { imageSize = ImageSize::Thumbnail; return std::move(*this); }
  Transaction&& onlyCache() && { loadingMode = LoadingMode::Cached; return std::move(*this); }
  Transaction&& callback(TransactionCallback && newCallback) &&;
  Transaction&& proactive() && { predictionMode = PredictionMode::Proactive; return std::move(*this); }
  Transaction&& persistent() && { persistency = Persistency::Yes; return std::move(*this); }

  Result execute() &&;

  QString toString() const;
  TransactionPtr cloneFor(pcontainer::ItemId const& id, unsigned const proactiveGeneration) const;

  QUuid const uuid;
  Priority priority;

  pcontainer::ItemId itemId;
  ImageSize imageSize = ImageSize::Full;
  LoadingMode loadingMode = LoadingMode::Active;
  PredictionMode predictionMode = PredictionMode::None;
  Persistency persistency = Persistency::No;
  OptTransactionCallback loadCallback;

private:
  Cache& cache;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_TRANSACTION_H_
