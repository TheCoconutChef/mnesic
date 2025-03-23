#include "mnesic/memoized.hpp"
#include <boost/ut.hpp>
#include <ranges>

int fib(int n)
{
  return n < 2 ? n : fib(n - 2) + fib(n - 1);
}

struct MemoizedFib
{
  long long int operator()(long long int n)
  {
    call_count++;
    return n <= 1 ? n : memoized(n - 2LL) + memoized(n - 1LL);
  }

  int call_count = 0;
  Memoized<MemoizedFib &> memoized{*this};
};

struct Foo
{
  mutable int call_count = 0;
  int operator()(int a, int b) const
  {
    call_count++;
    return a + b;
  }
};

struct MutableFoo
{
  int operator()(int a, int b)
  {
    return a + b;
  }
};

int free_func(int a, int b)
{
  return a - b;
}

using namespace boost::ut;
namespace rv = std::ranges::views;

int main()
{
  auto foo   = Foo();
  auto mem_f = Memoized(foo);
  auto a     = std::vector<int>(4);
  std::ranges::iota(a, 0);

  "no negative capacity"_test = [&]
  {
    expect(that % throws(
                      [&]
                      {
                        Memoized(foo, -1);
                      }));
  };

  "identical results"_test = [&]
  {
    for (auto i : a)
    {
      for (auto j : rv::iota(6, 8))
      {
        expect(that % foo(i, j) == mem_f(i, j));
      }
    }
  };

  "call count and cache size"_test = [&]
  {
    foo.call_count = 0;
    mem_f.clear_cache();

    mem_f(1, 1);
    mem_f(1, 1);
    mem_f(1, 1);
    mem_f(1, 1);

    expect(that % mem_f.entry_count() == 1u);
    expect(that % foo.call_count == 1u);
  };

  "capacity"_test = [&]
  {
    for (auto c : std::array{10, 50, 128})
    {
      auto m = Memoized(foo, c);
      expect(that % m.capacity() == c);
    }
  };

  "cache size and capacity"_test = [&]
  {
    foo.call_count = 0;
    auto mem_f     = Memoized(foo, 1);

    mem_f(0, 0);
    mem_f(1, 1);
    mem_f(2, 2);

    expect(that % 1 == mem_f.entry_count());
    expect(that % 3 == foo.call_count);
  };

  "static assert"_test = []
  {
    auto f        = Foo();
    const auto cf = Foo();
    auto mf       = Foo();

    auto mem_f  = Memoized(f);
    auto mem_cf = Memoized(cf);
    auto mem_rf = Memoized(Foo());
    auto mem_mf = Memoized(std::move(mf));

    static_assert(std::same_as<typename decltype(mem_f)::base_fn_t, Foo &>);
    static_assert(std::same_as<typename decltype(mem_cf)::base_fn_t, const Foo &>);
    static_assert(std::same_as<typename decltype(mem_rf)::base_fn_t, Foo>);
    static_assert(std::same_as<typename decltype(mem_mf)::base_fn_t, Foo>);
  };

  "free function"_test = []
  {
    auto mem_free = Memoized(&free_func);

    expect(that % mem_free(1, 2) == free_func(1, 2));
    expect(that % mem_free(2, 3) == free_func(2, 3));
    mem_free(1, 2);
    expect(that % mem_free.entry_count() == 2u);
  };

  "tuple hash identity"_test = []
  {
    auto t1     = std::make_tuple(1, 2, 3, 4);
    auto t2     = std::make_tuple(1, 2, 3, 4);
    auto t3     = std::make_tuple(1, 2, 3, 5);
    auto hasher = std::hash<std::tuple<int, int, int, int>>();

    expect(that % hasher(t1) == hasher(t2));
    expect(that % hasher(t1) != hasher(t3));
  };

  "fib"_test = []
  {
    auto mem_fib = MemoizedFib();
    auto n       = mem_fib(5);

    expect(mem_fib.memoized.entry_count() == 5);
    expect(n == fib(5));
  };

  return 0;
}
