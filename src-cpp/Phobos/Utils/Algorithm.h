#ifndef UTILS_ALGORITHM_H
#define UTILS_ALGORITHM_H

#include <algorithm>
#include <initializer_list>
#include <map>
#include <sstream>

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

template<typename OutContainer, typename InputIterator>
OutContainer moveFromRange(InputIterator begin, InputIterator end)
{
    OutContainer result;
    result.reserve(std::distance(begin, end));
    for (; begin != end; ++begin)
        result.insert(result.end(), std::move(*begin));
    return result;
}

template<typename ForwardIterator>
std::string joinString(ForwardIterator begin, ForwardIterator end, std::string const& separator)
{
    std::ostringstream buffer;
    for (ForwardIterator it = begin; it != end; ++it)
    {
        if (it != begin)
            buffer << separator;
        buffer << *it;
    }
    return buffer.str();
}

}} // namespace phobos::utils

#endif // UTILS_ALGORITHM_H
