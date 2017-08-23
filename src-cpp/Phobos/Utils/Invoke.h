#ifndef UTILS_INVOKE_H
#define UTILS_INVOKE_H

#include <functional>
#include <type_traits>

namespace phobos { namespace utils {

template<typename F, typename... Args>
auto invoke(F f, Args&&... args)
    -> decltype(std::ref(f)(std::forward<Args>(args)...))
{
    return std::ref(f)(std::forward<Args>(args)...);
}

}} // namespace phobos::utils

#endif // UTILS_INVOKE_H
