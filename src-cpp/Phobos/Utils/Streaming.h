#ifndef UTILS_STREAMING_H
#define UTILS_STREAMING_H

#include <string>
#include <type_traits>

namespace phobos { namespace utils { namespace stream {

namespace {

template <typename T> struct is_shared_ptr : public std::false_type {};
template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : public std::true_type {};
template <typename T> struct is_shared_ptr<std::shared_ptr<const T>> : public std::true_type {};

} // unnamed namespace

struct ObjId
{
    template <typename T>
    std::enable_if_t<std::is_pointer<T>::value, std::string>
    operator()(T const t_ptr) const
    {
        return std::to_string(std::uintptr_t(t_ptr));
    }

    template <typename T>
    std::enable_if_t<is_shared_ptr<T>::value, std::string>
    operator()(T const& t) const
    {
        return std::to_string(std::uintptr_t(t.get()));
    }

    template <typename T>
    std::enable_if_t<!is_shared_ptr<T>::value && !std::is_pointer<T>::value, std::string>
    operator()(T const& t) const
    {
        return std::to_string(std::uintptr_t(t.get()));
    }
};

}}} // namespace phobos::utils::stream

#endif // UTILS_STREAMING_H
