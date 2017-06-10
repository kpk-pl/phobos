#ifndef _UTILS_ASSERTED_H_
#define _UTILS_ASSERTED_H_

#include <memory>
#include <map>
#include <unordered_map>

namespace phobos { namespace utils { namespace asserted {

template<typename T, typename U, typename V>
auto fromMap(std::map<U, V> const& m, T const& key)
  -> decltype(m.begin()->second)
{
    auto const it = m.find(key);
    assert(it != m.end());
    return it->second;
}

template<typename T, typename U, typename V>
auto fromMap(std::unordered_map<U, V> const& m, T const& key)
  -> decltype(m.begin()->second)
{
    auto const it = m.find(key);
    assert(it != m.end());
    return it->second;
}

template<typename T>
T& fromPtr(T * t)
{
  assert(t);
  return(*t);
}

}}} // namespace phobos::utils::asserted

#endif // _UTILS_ASSERTED_H_
