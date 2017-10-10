#ifndef UTILS_ALGORITHM_H
#define UTILS_ALGORITHM_H

#include <algorithm>
#include <initializer_list>
#include <map>
#include <unordered_map>
#include <sstream>

namespace phobos { namespace utils {

template<typename T, typename U, typename V>
bool valueIn(T const& value, std::map<U, V> const& c)
{
  return c.find(value) != c.end();
}

template<typename T, typename U, typename V>
bool valueIn(T const& value, std::unordered_map<U, V> const& c)
{
  return c.find(value) != c.end();
}

template<typename T, typename U>
bool valueIn(T const& value, std::initializer_list<U> const& c)
{
    return std::find(c.begin(), c.end(), value) != c.end();
}

template<typename T, typename Container>
bool valueIn(T const& value, Container const& c)
{
  return std::find(std::begin(c), std::end(c), value) != std::end(c);
}

template<typename OutContainer, typename InputIterator>
OutContainer moveFromRange(OutContainer &output, InputIterator begin, InputIterator end)
{
  output.reserve(output.size() + std::distance(begin, end));
  std::move(begin, end, std::back_inserter(output));
  return output;
}

template<typename OutContainer, typename InputIterator>
OutContainer moveFromRange(InputIterator begin, InputIterator end)
{
  OutContainer output;
  return moveFromRange(output, begin, end);
}

template<typename OutContainer, typename InputIterator, typename Projection>
void transformToVector(OutContainer &out, InputIterator begin, InputIterator end, Projection && p)
{
  out.reserve(out.size() + std::distance(begin, end));
  std::transform(begin, end, std::back_inserter(out), std::forward<Projection>(p));
}

template<typename T, typename InputIterator, typename Projection>
std::vector<T> transformToVector(InputIterator begin, InputIterator end, Projection && p)
{
  std::vector<T> result;
  transformToVector(result, begin, end, std::forward<Projection>(p));
  return result;
}

}} // namespace phobos::utils

#endif // UTILS_ALGORITHM_H
