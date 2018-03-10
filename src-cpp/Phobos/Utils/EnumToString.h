#ifndef UTILS_ENUMTOSTRING_H
#define UTILS_ENUMTOSTRING_H

namespace phobos { namespace utils {

template<typename T, unsigned N>
struct EnumToString
{
  constexpr explicit EnumToString()
  {}

  constexpr EnumToString& operator()(T val, const char* str)
  {
    typeMap[count] = val;
    strMap[count++] = str;
    return *this;
  }

  constexpr const char* operator()(T val) const
  {
    for (unsigned i = 0; i < count; ++i)
      if (typeMap[i] == val)
        return strMap[i];

    throw "Tried to access not registered enum";
  }

  T typeMap[N] = {};
  const char* strMap[N] = {};

private:
  unsigned count = 0;
};

}} // namespace phobos::utils

#endif // UTILS_ENUMTOSTRING_H
