#ifndef IMAGECACHE_TRANSACTION_H_
#define IMAGECACHE_TRANSACTION_H_

#include "ImageCache/CacheFwd.h"
#include "ImageCache/TransactionFwd.h"
#include "PhotoContainers/ItemId.h"
#include <QImage>
#include <QUuid>
#include <functional>

namespace phobos { namespace icache {

namespace detail {
} // namespace detail

struct TransactionGroup;

class Transaction
{
public:
  enum class ImageQuality
  {
    None, Blank, ExifThumb, Thumb, Full
  };

  struct Result
  {
    QImage image;
    ImageQuality quality;
  };

  using CallbackType = std::function<void(Result const&)>;

  struct OptCallback
  {
    void operator()(Result && result) const;
    void operator()(QImage && im, ImageQuality qual) const;
    CallbackType func;
  };

  Transaction(Cache& cache);

  Transaction&& item(pcontainer::ItemId const& itemId) && { this->itemId = itemId; return std::move(*this); }
  Transaction&& thumbnail() && { onlyThumbnail = true; return std::move(*this); }
  Transaction&& onlyCache() && { disableLoading = true; return std::move(*this); }
  Transaction&& callback(CallbackType && newCallback) &&;

  Result execute() &&;
  Result operator()() const;

  bool shouldStartThread() const { return _shouldStartThread && !disableLoading; }

  QString toString() const;

  QUuid const uuid;
  pcontainer::ItemId const& getItemId() const { return itemId; }
  OptCallback const& getCallback() const { return loadCallback; }
  bool isThumbnail() const { return onlyThumbnail; }

private:
  Cache& cache;

  pcontainer::ItemId itemId;
  bool onlyThumbnail = false;
  bool disableLoading = false;
  OptCallback loadCallback;

  bool mutable _shouldStartThread = false;
};

struct TransactionGroup
{
  using Result = std::map<pcontainer::ItemId, Transaction::Result>;

  Result operator()() const;
  TransactionGroup& operator+=(Transaction && t);
  TransactionGroup& operator+=(Transaction const& t);

  TransactionVec transactions;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_TRANSACTION_H_
