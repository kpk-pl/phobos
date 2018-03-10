#ifndef _UTILS_ASSERTED_H_
#define _UTILS_ASSERTED_H_

#include <memory>
#include <map>
#include <cassert>
#include <unordered_map>

namespace phobos { namespace utils { namespace asserted {

template<typename T, typename U, typename V>
V const& fromMap(std::map<U, V> const& m, T const& key)
{
  auto const it = m.find(key);
  assert(it != m.end());
  return it->second;
}

template<typename T, typename U, typename V>
V const& fromMap(std::unordered_map<U, V> const& m, T const& key)
{
  auto const it = m.find(key);
  assert(it != m.end());
  return it->second;
}

template<typename T>
auto fromPtr(T const& t) -> decltype(*t)
{
  assert(t);
  return(*t);
}

namespace detail {
struct AlwaysAsserted
{
  template<typename T>
  operator T& () const
  {
    assert(false);
    __builtin_unreachable();
  }
};
} // namespace detail

extern const detail::AlwaysAsserted always;

}}} // namespace phobos::utils::asserted

#endif // _UTILS_ASSERTED_H_
