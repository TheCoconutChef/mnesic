#ifndef _MNESIC_HPP_INCLUDED_
#define _MNESIC_HPP_INCLUDED_

#include <cstddef>
#include <functional>
#include <list>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <unordered_map>

// callable traits
namespace detail
{
template <typename T, typename = void> struct Return
{
};

template <typename Ret, typename... Args> struct Return<Ret(Args...)>
{
  using type = Ret;
};

template <typename Ret, typename... Args> struct Return<Ret (*)(Args...)>
{
  using type = Ret;
};

template <typename Class, typename Ret, typename... Args> struct Return<Ret (Class::*)(Args...)>
{
  using type = Ret;
};

template <typename Class, typename Ret, typename... Args> struct Return<Ret (Class::*)(Args...) const>
{
  using type = Ret;
};

template <typename T> struct Return<T, std::void_t<decltype(&T::operator())>> : Return<decltype(&T::operator())>
{
};

template <typename T> using return_t = typename Return<T>::type;

} // namespace detail

// tuple hash
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
  constexpr auto hasher = std::hash<T>{};
  seed.val ^= hash_box{hasher(v)} + 0x9e3779b9 + (seed.val << 6) + (seed.val >> 2);
  return seed;
}

struct pack_hash
{
  template <typename... Ts> constexpr std::size_t operator()(const Ts &...ts) const noexcept
  {
    constexpr auto hasher = std::hash<int>();
    const auto init       = hash_box{.val = hasher(0)};
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

// lru cache
namespace detail
{
template <typename key_t, typename value_t> class lru_cache
{
public:
  using key_value_pair_t = std::pair<key_t, value_t>;
  using list_iterator_t  = typename std::list<key_value_pair_t>::iterator;

  lru_cache(std::size_t max_size = 128) : _max_size(max_size)
  {
  }

  const value_t &put(const key_t &key, const value_t &value)
  {
    auto it = _cache_items_map.find(key);
    _cache_items_list.push_front(key_value_pair_t(key, value));
    const auto &ref = _cache_items_list.front().second;
    if (it != _cache_items_map.end())
    {
      _cache_items_list.erase(it->second);
      _cache_items_map.erase(it);
    }
    _cache_items_map[key] = _cache_items_list.begin();

    if (_cache_items_map.size() > _max_size)
    {
      const auto last = std::prev(_cache_items_list.end());
      _cache_items_map.erase(last->first);
      _cache_items_list.pop_back();
    }
    return ref;
  }

  std::optional<std::reference_wrapper<value_t>> safe_get(const key_t &key)
  {
    auto it = _cache_items_map.find(key);
    if (it == _cache_items_map.end())
      return {};

    _cache_items_list.splice(_cache_items_list.begin(), _cache_items_list, it->second);
    return it->second->second;
  }
  const value_t &get(const key_t &key)
  {
    auto it = _cache_items_map.find(key);
    if (it == _cache_items_map.end())
    {
      throw std::range_error("There is no such key in cache");
    }
    else
    {
      _cache_items_list.splice(_cache_items_list.begin(), _cache_items_list, it->second);
      return it->second->second;
    }
  }

  bool exists(const key_t &key) const
  {
    return _cache_items_map.find(key) != _cache_items_map.end();
  }

  std::size_t size() const
  {
    return _cache_items_map.size();
  }

  std::size_t capacity() const
  {
    return _max_size;
  }

  void clear()
  {
    _cache_items_list.clear();
    _cache_items_map.clear();
  }

private:
  std::list<key_value_pair_t> _cache_items_list;
  std::unordered_map<key_t, list_iterator_t> _cache_items_map;
  std::size_t _max_size;
};

} // namespace detail

template <typename Fn> class Memoized
{
  Fn fn;

  using fn_t     = std::decay_t<Fn>;
  using return_t = detail::return_t<fn_t>;

  // TODO replace by caching policy
  detail::lru_cache<std::size_t, return_t> cache{128};

public:
  using base_fn_t = Fn;

  explicit Memoized(Fn &&fn, int cache_size = 128)
      : fn(std::forward<Fn>(fn)), cache{static_cast<std::size_t>(cache_size)}
  {
    if (cache_size < 0)
    {
      throw std::domain_error{"lru cache capacity must be >= 0"};
    }
  }

  template <typename... Args> const return_t &operator()(Args &&...args)
  {
    // TODO: use args_t somehow to make sure that what would be implicitly converted
    // to U from T in the original function calls also gets implicitly converted in
    // the cache lookup.
    //
    // This is because a call such as mem_f("foo") memoizing a call f(const std::string&)
    // is gonna be interpreted as a const char* arg which isn't hashable whereas std::string
    // is. Hence mem_f won't be callable in the same manner as f, which breaks expectations.

    const auto &h       = detail::pack_hash{}(std::forward<Args>(args)...);
    const auto &val_opt = cache.safe_get(h);

    if (val_opt.has_value())
    {
      return val_opt.value();
    }

    return cache.put(h, fn(std::forward<Args>(args)...));
  }

  void clear_cache()
  {
    return cache.clear();
  }

  std::size_t entry_count() const
  {
    return cache.size();
  }

  std::size_t capacity() const
  {
    return cache.capacity();
  }
};

template <typename Fn> Memoized(const Fn &, int) -> Memoized<const Fn &>;
template <typename Fn> Memoized(const Fn &) -> Memoized<const Fn &>;
template <typename Fn> Memoized(Fn &, int) -> Memoized<Fn &>;
template <typename Fn> Memoized(Fn &) -> Memoized<Fn &>;
template <typename Fn> Memoized(Fn &&, int) -> Memoized<Fn>;
template <typename Fn> Memoized(Fn &&) -> Memoized<Fn>;

#endif // !_MNESIC_HPP_INCLUDED_
