#ifndef _TUPL_HASH_INCLUDED_
#define _TUPL_HASH_INCLUDED_

#include <functional>
#include <tuple>

namespace detail {
#define RIGHT_FOLD_MAP(FN, PACK) ((FN(PACK)), ...)

template <typename Fn, typename... Ts>
auto right_fold_map(Fn &&fn, const Ts &...ts) {
  RIGHT_FOLD_MAP(fn, ts);
}

template <typename Fn, typename... Ts>
auto tuple_it(Fn fn, const std::tuple<Ts...> &tup) {
  return std::apply(right_fold_map, tup);
}

struct hash_box {
  hash_box(std::size_t v) : val{v} {}
  std::size_t val{0};
  inline operator std::size_t() { return val; }
};

template <typename T> hash_box operator&(hash_box seed, const T &v) {
  std::hash<T> hasher{};
  seed.val ^=
      hash_box{hasher(v)} + 0x9e3779b9 + (seed.val << 6) + (seed.val >> 2);
  return seed;
}

struct pack_hash {
  template <typename... Ts>
  std::size_t operator()(const Ts &...ts) const noexcept {
    constexpr auto h = std::hash<int>{};
    const hash_box init = h(0);
    return (init & ... & ts);
  }
};

struct tuple_hash {
  template <typename... Ts>
  std::size_t operator()(const std::tuple<Ts...> &tup) const noexcept {
    return std::apply(pack_hash{}, tup);
  }
};
} // namespace detail

// Tuple hash std namespace injection
template <typename... Ts> struct std::hash<std::tuple<Ts...>> {
  std::size_t operator()(const std::tuple<Ts...> &tup) const noexcept {
    return detail::tuple_hash{}(tup);
  }
};

#endif // ! _TUPL_HASH_INCLUDED_
