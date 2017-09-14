#ifndef IMAGECACHE_TRANSACTION_H_
#define IMAGECACHE_TRANSACTION_H_

#include "ImageCache/CacheFwd.h"
#include "ImageCache/TransactionFwd.h"
#include "PhotoContainers/ItemId.h"
#include <QImage>
#include <QUuid>

namespace phobos { namespace icache {

struct TransactionGroup;

class Transaction
{
public:
  struct Factory
  {
    static Transaction singlePhoto(Cache & cache, pcontainer::ItemId const& itemId);
    static Transaction singlePhotoStatic(Cache & cache, pcontainer::ItemId const& itemId);
    static TransactionGroup seriesPhotos(Cache & cache, QUuid const& seriesId);
  };

  Transaction(Cache& cache);

  Transaction&& item(pcontainer::ItemId const& itemId) && { this->itemId = itemId; return std::move(*this); }
  Transaction&& thumbnail() && { onlyThumbnail = true; return std::move(*this); }
  Transaction&& onlyCache() && { disableLoading = true; return std::move(*this); }

  QImage execute() &&;
  QImage operator()() const;

  bool shouldStartThread() const { return _shouldStartThread && !disableLoading; }

  QString toString() const;

  QUuid const uuid;
  pcontainer::ItemId const& getItemId() const { return itemId; }

private:
  Cache& cache;

  pcontainer::ItemId itemId;
  bool onlyThumbnail = false;
  bool disableLoading = false;

  bool mutable _shouldStartThread = false;
};

struct TransactionGroup
{
  using Result = std::map<pcontainer::ItemId, QImage>;

  Result operator()() const;
  TransactionGroup& operator+=(Transaction && t);
  TransactionGroup& operator+=(Transaction const& t);

  TransactionVec transactions;
};


}} // namespace phobos::icache

#endif // IMAGECACHE_TRANSACTION_H_
