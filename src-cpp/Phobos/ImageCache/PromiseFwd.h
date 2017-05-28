#ifndef PHOBOS_IMAGECACHE_PROMISEFWD_H_
#define PHOBOS_IMAGECACHE_PROMISEFWD_H_

#include <memory>
#include <vector>

namespace phobos { namespace icache {

class Promise;
using PromisePtr = std::shared_ptr<Promise>;
using PromisePtrVec = std::vector<PromisePtr>;

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_PROMISEFWD_H_