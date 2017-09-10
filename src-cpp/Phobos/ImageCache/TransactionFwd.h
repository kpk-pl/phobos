#ifndef IMAGECACHE_TRANSACTION_FWD_H_
#define IMAGECACHE_TRANSACTION_FWD_H_

#include <vector>

namespace phobos { namespace icache {

class Transaction;
using TransactionVec = std::vector<Transaction>;

struct TransactionGroup;

}} // namespace phobos::icache

#endif // IMAGECACHE_TRANSACTION_FWD_H_
