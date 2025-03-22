#ifndef _LRU_CACHE_HPP_INCLUDED_
#define _LRU_CACHE_HPP_INCLUDED_

#include <list>
#include <optional>
#include <stdexcept>
#include <unordered_map>

template <typename key_t, typename value_t> class lru_cache
{
public:
  using key_value_pair_t = std::pair<key_t, value_t>;
  using list_iterator_t  = typename std::list<key_value_pair_t>::iterator;

  lru_cache(std::size_t max_size = 128) : _max_size(max_size)
  {
  }

  void put(const key_t &key, const value_t &value)
  {
    auto it = _cache_items_map.find(key);
    _cache_items_list.push_front(key_value_pair_t(key, value));
    if (it != _cache_items_map.end())
    {
      _cache_items_list.erase(it->second);
      _cache_items_map.erase(it);
    }
    _cache_items_map[key] = _cache_items_list.begin();

    if (_cache_items_map.size() > _max_size)
    {
      auto last = _cache_items_list.end();
      last--;
      _cache_items_map.erase(last->first);
      _cache_items_list.pop_back();
    }
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

#endif // !_MNESIC_HPP_INCLUDED_
