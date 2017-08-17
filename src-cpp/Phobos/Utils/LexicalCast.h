#ifndef UTILS_LEXICALCAST_H
#define UTILS_LEXICALCAST_H

#include <sstream>
#include <sstream>

namespace phobos { namespace utils {

template<typename T>
std::string lexicalCast(T const& rhs)
{
  std::ostringstream ss;
  ss << rhs;
  return ss.str();
}

}} // namespace phobos::utils

#endif // UTILS_LEXICALCAST_H
