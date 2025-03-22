#include <fmt/base.h>

#include "mnesic/memoized.hpp"

int naive_fib_call_count = 0;
long long int fib_naive(long long int n)
{
  naive_fib_call_count++;
  return n <= 1 ? n : fib_naive(n - 1) + fib_naive(n - 2);
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

int main()
{
  auto fib = MemoizedFib();
  // Call to recursively defined free function
  for (auto n = 20LL; n <= 30LL; ++n)
  {
    fmt::println("F_{} = {}", n, fib(n));
  }
  fmt::println("F_10 = {}", fib_naive(10));
  fmt::println("mem_fib_call_count: {}", fib.call_count);
  fmt::println("naive_fib_call_count: {}", naive_fib_call_count);

  return 0;
}
