#ifndef UTILS_COMPARATORS_H
#define UTILS_COMPARATORS_H

#include "Utils/Invoke.h"

namespace phobos { namespace utils {

namespace detail {
namespace cont_traits {
struct equality
{
  explicit equality() = default;
  template<typename Order, typename Res, typename T, typename U>
  bool operator()(Order const&, Res && base, T &&, U &&) const
  {
    return base;
  }
};

struct less
{
  explicit less() = default;
  template<typename Order, typename Res, typename T, typename U>
  bool operator()(Order const& order, Res const& base, T && lhs, U && rhs) const
  {
    auto const gt = invoke(order, std::forward<T>(rhs), std::forward<U>(lhs));
    return !base && !gt;
  }
};

template<typename Order>
struct pred
{
  using type = typename pred<typename Order::base_order>::type;
};
template<>
struct pred<std::less<>>
{
  using type = less;
};
template<>
struct pred<std::equal_to<>>
{
  using type = equality;
};

template<typename Order>
using pred_t = typename pred<Order>::type;
} // namespace cont_traits

template <typename StrictOrder1, typename StrictOrder2>
inline auto order_then(StrictOrder1&& so1, StrictOrder2&& so2);

template <typename StrictOrder, typename Projection>
inline auto order_on(StrictOrder&& so, Projection&& p);

template <typename SO1, typename SO2>
struct order_then_t
{
  static_assert(std::is_same<typename SO1::result_type, typename SO2::result_type>::value, "Result types must be the same");
  using result_type = typename SO1::result_type;
  using base_order = typename SO2::base_order; // second type's order, since this makes tailing more understandable

  explicit order_then_t(SO1 so1, SO2 so2) : so1(std::move(so1)), so2(std::move(so2))
  {}

  template<typename T, typename U>
  inline auto operator()(T && lhs, U && rhs) const
  {
    auto const base = invoke(so1, lhs, rhs);
    if (invoke(cont_traits::pred_t<SO1>{}, so1, base, lhs, rhs))
      return invoke(so2, std::forward<T>(lhs), std::forward<U>(rhs));

    return base;
  }

  template <typename StrictOrder>
  auto then(StrictOrder && order) const
  {
    return order_then(*this, std::forward<StrictOrder>(order));
  }

  template <typename Projection>
  auto then_on(Projection && p) const
  {
    auto sec_order = order_on(base_order{}, std::forward<Projection>(p));
    return order_then(*this, std::move(sec_order));
  }

private:
  SO1 so1;
  SO2 so2;
};

template <typename StrictOrder1, typename StrictOrder2>
inline auto order_then(StrictOrder1&& so1, StrictOrder2&& so2)
{
  using OrderT = detail::order_then_t<std::decay_t<StrictOrder1>, std::decay_t<StrictOrder2> >;
  return OrderT(std::forward<StrictOrder1>(so1), std::forward<StrictOrder2>(so2));
}

template <typename StrictOrder, typename Projection>
struct order_on_t
{
  using result_type = typename StrictOrder::result_type;
  using base_order = StrictOrder;

  explicit order_on_t(StrictOrder so, Projection p) :
    order(std::move(so)), projection(std::move(p))
  {}

  template<typename T, typename U>
  result_type operator()(T && lhs, U && rhs) const
  {
    return invoke(order,
                  invoke(projection, std::forward<T>(lhs)),
                  invoke(projection, std::forward<U>(rhs)));
  }

  template <typename SecondaryStrictOrder>
  auto then(SecondaryStrictOrder && order) const
  {
    return order_then(*this, std::forward<SecondaryStrictOrder>(order));
  }

  template <typename OtherProjection>
  auto then_on(OtherProjection && p) const
  {
    auto sec_order = order_on(base_order{}, std::forward<OtherProjection>(p));
    return order_then(*this, std::move(sec_order));
  }

private:
  StrictOrder order;
  Projection projection;
};

template <typename StrictOrder, typename Projection>
inline auto order_on(StrictOrder&& so, Projection&& p)
{
  using OrderT = detail::order_on_t<std::decay_t<StrictOrder>, std::decay_t<Projection> >;
  return OrderT(std::forward<StrictOrder>(so), std::forward<Projection>(p));
}

template<typename Order>
struct order_t
{
  using result_type = bool;
  using base_order = Order;

  explicit order_t() = default;

  template <typename T, typename U>
  result_type operator()(T && t, U && u) const
  {
    return invoke(order, std::forward<T>(t), std::forward<U>(u));
  }

  template <typename Projection>
  auto on(Projection&& p) const
  {
    return order_on(*this, std::forward<Projection>(p));
  }

private:
  Order order;
};
} // namespace detail

inline auto less()
{
  return detail::order_t<std::less<>>{};
}

inline auto equal()
{
  return detail::order_t<std::equal_to<>>{};
}

}} // namespace phobos::utils

#endif // UTILS_COMPARATORS_H
