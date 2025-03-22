#ifndef _MNESIC_HPP_INCLUDED_
#define _MNESIC_HPP_INCLUDED_

#include "detail/lru_cache.hpp"
#include "detail/tuple_hash.hpp"
#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/return_type.hpp>

#include <cstddef>
#include <stdexcept>
#include <type_traits>

template <typename Fn> class Memoized
{
  using fn_t     = std::remove_cvref_t<Fn>;
  using return_t = boost::callable_traits::return_type_t<fn_t>;

  // TODO some other kind of storage policy
  //
  // What if we have const T&? What if we have T&&? What about just T?
  Fn fn;

  // TODO replace by caching policy
  lru_cache<std::size_t, return_t> cache{128};

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

  template <typename... Args> return_t operator()(Args &&...args)
  {
    // TODO: use args_t somehow to make sure that what would be implicitly converted
    // to U from T in the original function calls also gets implicitly converted in
    // the cache lookup.
    //
    // This is because a call such as mem_f("foo") memoizing a call f(const std::string&)
    // is gonna be interpreted as a const char* arg which isn't hashable whereas std::string
    // is. Hence mem_f won't be callable in the same manner as f, which breaks expectations.
    //
    // the call to args_t
    //    using args_t   = boost::callable_traits::args_t<fn_t>;

    const auto &h       = detail::pack_hash{}(std::forward<Args>(args)...);
    const auto &val_opt = cache.safe_get(h);

    if (val_opt.has_value())
    {
      return val_opt.value();
    }

    auto res = fn(std::forward<Args>(args)...);
    cache.put(h, res);
    return res;
  }

  void clear_cache()
  {
    return cache.clear();
  }

  std::size_t record_count() const
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
