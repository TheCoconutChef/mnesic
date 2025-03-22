#define ANKERL_NANOBENCH_IMPLEMENT

#include "mnesic/memoized.hpp"
#include <nanobench.h>

int fib(int n)
{
  return n < 2 ? n : fib(n - 2) + fib(n - 1);
}

// TODO: improve this recursive case it's kind of basic
int fib_call_count = 0;
long long int fib2(long long int n);
auto memoized = Memoized(fib2);
long long int fib2(long long int n)
{
  fib_call_count++;
  return n <= 1 ? n : memoized(n - 1LL) + memoized(n - 2LL);
}

using namespace ankerl;

int main()
{
  auto bench = nanobench::Bench().minEpochIterations(70000);

  const auto n = 20;

  bench.run("memoized fib",
            []
            {
              nanobench::doNotOptimizeAway(fib2(n));
              memoized.clear_cache();
            });
  bench.run("naive fib",
            [n]
            {
              nanobench::doNotOptimizeAway(fib(n));
            });

  return 0;
}
