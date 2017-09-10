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
    static Transaction singleThumbnail(Cache & cache, pcontainer::ItemId const& itemId);
    static TransactionGroup seriesPhotos(Cache & cache, QUuid const& seriesId);
    static TransactionGroup seriesThumbnails(Cache & cache, QUuid const& seriesId);
  };

  Transaction(Cache const& cache, pcontainer::ItemId const& itemId);

  QImage operator()() const;
  bool startThread() const { return shouldStartThread; }

  QUuid const uuid;
  pcontainer::ItemId const itemId;

private:
  enum class Type { Full, Thumbnail };
  Transaction& setType(Type const type) & { this->type = type; return *this; }
  Transaction&& setType(Type const type) && { this->type = type; return std::move(*this); }


  Cache const& cache;
  Type type = Type::Full;
  bool mutable shouldStartThread = false;
};

struct TransactionGroup
{
  using Result = std::map<pcontainer::ItemId, QImage>;

  Result operator()() const;
  TransactionGroup& operator+=(Transaction && t);

  TransactionVec transactions;
};


}} // namespace phobos::icache

#endif // IMAGECACHE_TRANSACTION_H_
