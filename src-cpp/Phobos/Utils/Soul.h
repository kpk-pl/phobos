#ifndef UTILS_SOUL_H_
#define UTILS_SOUL_H_

#include <memory>
#include <type_traits>

namespace phobos { namespace utils {

namespace detail {
template<typename T>
struct PtrToPtr
{
  PtrToPtr(T && init) : _ptr(std::move(init))
  {}
  auto operator*() const noexcept(noexcept(**std::declval<T>()))
  {
    // maybe nullptr dereference
    return **_ptr;
  }
  auto operator->() const noexcept(noexcept(*std::declval<T>()))
  {
    return bool(*this) ? *_ptr : nullptr;
  }
  operator bool() const noexcept(noexcept(bool(std::declval<T>())) && noexcept(bool(*std::declval<T>())))
  {
    return static_cast<bool>(_ptr) && static_cast<bool>(*_ptr);
  }
private:
  T _ptr;
};

template<typename T>
auto makePtrToPtr(T && t)
{
  return PtrToPtr<T>(std::forward<T>(t));
}

template<typename T>
struct Lifetime
{
  Lifetime(std::shared_ptr<T> const& shPtr) : weakPtr(shPtr)
  {}

  auto lock() const
  {
    return makePtrToPtr(weakPtr.lock());
  }
private:
  std::weak_ptr<T> weakPtr;
};
} // unnamed namespace

template<typename T>
class Soul
{
public:
  using TPtr = T*;
  using Lifetime = detail::Lifetime<TPtr>;

  explicit Soul(TPtr const object) :
    _lifetime(std::make_shared<TPtr>(object))
  {}

  Lifetime lifetime() const
  {
    return Lifetime(_lifetime);
  }

private:
  std::shared_ptr<TPtr> _lifetime;
};

}} // namespace phobos::utils

#endif // UTILS_SOUL_H_
