#ifndef _TUPL_HASH_INCLUDED_
#define _TUPL_HASH_INCLUDED_

#include <functional>
#include <tuple>

namespace detail
{
template <typename Fn, typename... Ts> constexpr auto right_fold_map(Fn &&fn, const Ts &...ts)
{
  ((std::forward<Fn>(ts)), ...);
}

template <typename Fn, typename... Ts> constexpr auto tuple_it(Fn fn, const std::tuple<Ts...> &tup)
{
  return std::apply(right_fold_map, tup);
}

struct hash_box
{
  std::size_t val = 0;

  constexpr inline operator std::size_t()
  {
    return val;
  }
};

template <typename T> constexpr hash_box operator&(hash_box seed, const T &v)
{
  constexpr auto hasher = std::hash<T>();
  seed.val ^= hash_box{hasher(v)} + 0x9e3779b9 + (seed.val << 6) + (seed.val >> 2);
  return seed;
}

struct pack_hash
{
  template <typename... Ts> constexpr std::size_t operator()(const Ts &...ts) const noexcept
  {
    constexpr auto hasher = std::hash<int>();
    const auto init       = hash_box(hasher(0));
    return (init & ... & ts);
  }
};

struct tuple_hash
{
  template <typename... Ts> constexpr std::size_t operator()(const std::tuple<Ts...> &tup) const noexcept
  {
    return std::apply(pack_hash(), tup);
  }
};
} // namespace detail

// Tuple hash std namespace injection
template <typename... Ts> struct std::hash<std::tuple<Ts...>>
{
  constexpr std::size_t operator()(const std::tuple<Ts...> &tup) const noexcept
  {
    return detail::tuple_hash()(tup);
  }
};

#endif // ! _TUPL_HASH_INCLUDED_
