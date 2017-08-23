#ifndef UTILS_COMPARATORS_H
#define UTILS_COMPARATORS_H

#include "Utils/Invoke.h"

namespace phobos { namespace utils {

namespace detail {

template <typename StrictOrder, typename Projection>
struct order_on_t
{
  explicit order_on_t() = default;
  order_on_t(StrictOrder const& so, Projection const& p) :
      order(so), projection(p)
  {}

  template<typename T, typename U>
  inline auto operator()(T&& lhs, U&& rhs) const
  {
    return invoke(order,
                  invoke(projection, std::forward<T>(lhs)),
                  invoke(projection, std::forward<U>(rhs)));
  }

private:
  StrictOrder order;
  Projection projection;
};
} // namespace detail

template <typename StrictOrder, typename Projection>
inline auto order_on(StrictOrder&& so, Projection&& p)
{
  using OrderT = detail::order_on_t<std::decay_t<StrictOrder>, std::decay_t<Projection> >;
  return OrderT(std::forward<StrictOrder>(so), std::forward<Projection>(p));
}

namespace detail {
struct less_t
{
  explicit less_t() = default;
  using result_type = bool;

  template <typename T, typename U>
  result_type operator()(T const& t, U const& u) const
  {
    return t < u;
  }

  template <typename Projection>
  auto on(Projection&& p) const
  {
    return order_on(*this, std::forward<Projection>(p));
  }
};

struct equal_t
{
  explicit equal_t() = default;
  using result_type = bool;

  template <typename T, typename U>
  result_type operator()(T const& t, U const& u) const
  {
    return t == u;
  }

  template <typename Projection>
  auto on(Projection&& p) const
  {
    return order_on(*this, std::forward<Projection>(p));
  }
};
} // namespace detail

inline auto less()
{
  return detail::less_t{};
}

inline auto equal()
{
  return detail::equal_t{};
}

}} // namespace phobos::utils

#endif // UTILS_COMPARATORS_H
