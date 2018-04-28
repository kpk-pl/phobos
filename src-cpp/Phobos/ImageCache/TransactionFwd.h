#ifndef IMAGECACHE_TRANSACTION_FWD_H_
#define IMAGECACHE_TRANSACTION_FWD_H_

#include "ImageCache/CacheFwd.h"
#include <vector>
#include <memory>
#include <functional>

namespace phobos { namespace icache {

class Transaction;
using TransactionPtr = std::shared_ptr<Transaction>;
using ConstTransactionPtr = std::shared_ptr<const Transaction>;
using TransactionVec = std::vector<Transaction>;

using TransactionCallback = std::function<void(Result const&)>;

struct OptTransactionCallback
{
  void operator()(Result && result) const
  {
    if (func)
      func(std::move(result));
  }
  TransactionCallback func;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_TRANSACTION_FWD_H_
