#ifndef UTILS_ALGORITHM_H
#define UTILS_ALGORITHM_H

#include <algorithm>
#include <initializer_list>
#include <map>

namespace phobos { namespace utils {

template<typename T, typename U>
bool valueIn(T const& value, std::initializer_list<U> const& c)
{
    return std::find(c.begin(), c.end(), value) != c.end();
}

template<typename T, typename U, typename V, typename Alloc>
bool valueIn(T const& value, std::map<U, V, Alloc> const& c)
{
    return c.find(value) != c.end();
}

}} // namespace phobos::utils

#endif // UTILS_ALGORITHM_H
