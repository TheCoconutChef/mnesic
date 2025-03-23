#include "mnesic/memoized.hpp"
#include <fmt/base.h>
#include <fmt/ranges.h>

std::size_t next_collatz(std::size_t n)
{
  if (n % 2 == 0)
  {
    return n / 2;
  }
  else
  {
    return 3 * n + 1;
  }
}

std::vector<std::size_t> collatz(std::size_t n)
{
  auto v = std::vector<std::size_t>{n};

  for (auto m = next_collatz(n); m > 1u; m = next_collatz(m))
  {
    v.push_back(m);
  }

  return v;
}

int main()
{
  auto m = Memoized(collatz);

  const auto &a = m(7);
  const auto &b = m(7);

  fmt::println("collatz(7) = {}", a);
  fmt::println("collatz(7) = {}", b);
  fmt::println("entry count: {}", m.entry_count());

  return 0;
}
