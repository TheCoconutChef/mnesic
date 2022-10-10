#ifndef _MNESIC_HPP_INCLUDED_
#define _MNESIC_HPP_INCLUDED_

#include "detail/function_traits.hpp"
#include "detail/tuple_hash.hpp"
#include <optional>

#include <cstddef>
#include <list>
#include <stdexcept>
#include <unordered_map>

template <typename key_t, typename value_t> class lru_cache {
public:
  typedef typename std::pair<key_t, value_t> key_value_pair_t;
  typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;

  lru_cache(std::size_t max_size = 128) : _max_size(max_size) {}

  void put(const key_t &key, const value_t &value) {
    auto it = _cache_items_map.find(key);
    _cache_items_list.push_front(key_value_pair_t(key, value));
    if (it != _cache_items_map.end()) {
      _cache_items_list.erase(it->second);
      _cache_items_map.erase(it);
    }
    _cache_items_map[key] = _cache_items_list.begin();

    if (_cache_items_map.size() > _max_size) {
      auto last = _cache_items_list.end();
      last--;
      _cache_items_map.erase(last->first);
      _cache_items_list.pop_back();
    }
  }

  std::optional<std::reference_wrapper<value_t>> safe_get(const key_t &key) {
    auto it = _cache_items_map.find(key);
    if (it == _cache_items_map.end())
      return {};

    _cache_items_list.splice(_cache_items_list.begin(), _cache_items_list,
                             it->second);
    return it->second->second;
  }
  const value_t &get(const key_t &key) {
    auto it = _cache_items_map.find(key);
    if (it == _cache_items_map.end()) {
      throw std::range_error("There is no such key in cache");
    } else {
      _cache_items_list.splice(_cache_items_list.begin(), _cache_items_list,
                               it->second);
      return it->second->second;
    }
  }

  bool exists(const key_t &key) const {
    return _cache_items_map.find(key) != _cache_items_map.end();
  }

  std::size_t size() const { return _cache_items_map.size(); }

  std::size_t capacity() const { return _max_size; }

  void clear() {
    _cache_items_list.clear();
    _cache_items_map.clear();
  }

private:
  std::list<key_value_pair_t> _cache_items_list;
  std::unordered_map<key_t, list_iterator_t> _cache_items_map;
  std::size_t _max_size;
};

template <typename Fn> class Memoized {
  using return_t = return_of_t<Fn>;
  using args_t = args_of_t<Fn>;

  Fn &fn;
  // TODO replace by caching policy
  lru_cache<std::size_t, return_t> cache{128};

public:
  explicit Memoized(Fn &fn, int cache_size = 128)
      : fn{fn}, cache{static_cast<std::size_t>(cache_size)} {
    if (cache_size < 0)
      throw std::domain_error{"lru cache capacity must be >= 0"};
  }

  template <typename... Args> return_t operator()(Args... args) {
    const auto h = detail::pack_hash{}(args...);
    const auto val_opt = cache.safe_get(h);

    if (val_opt.has_value())
      return val_opt.value();

    auto res = fn(args...);
    cache.put(h, res);
    return res;
  }

  void clear_cache() { return cache.clear(); }

  std::size_t record_count() const { return cache.size(); }

  std::size_t capacity() const { return cache.capacity(); }
};

#endif // !_MNESIC_HPP_INCLUDED_
