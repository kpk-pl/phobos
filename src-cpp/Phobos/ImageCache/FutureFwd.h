#ifndef IMAGE_CACHE_FUTURE_FWD_H_
#define IMAGE_CACHE_FUTURE_FWD_H_

#include <memory>
#include <vector>

namespace phobos { namespace icache {

class Future;
using FuturePtr = std::shared_ptr<Future>;
using FuturePtrVec = std::vector<FuturePtr>;

}} // namespace phobos::icache

#endif // IMAGE_CACHE_FUTURE_FWD_H_
