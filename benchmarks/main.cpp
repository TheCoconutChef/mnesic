#define ANKERL_NANOBENCH_IMPLEMENT

#include "mnesic/memoized.hpp"
#include <nanobench.h>

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

using namespace ankerl;

int main()
{
  auto bench = nanobench::Bench().title("Naive fibonacci vs memoized").minEpochIterations(70000).relative(true);

  const auto n = 20;

  bench.run("naive fib",
            [n]
            {
              nanobench::doNotOptimizeAway(fib(n));
            });
  bench.run("memoized fib",
            [n]() mutable
            {
              auto f = MemoizedFib();
              nanobench::doNotOptimizeAway(f(n));
            });

  return 0;
}
