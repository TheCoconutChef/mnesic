#include "mnesic/memoized.hpp"
#include <fmt/base.h>

int foo(int a, int b)
{
  fmt::println("Foo is called with {}, {}", a, b);
  return a + b;
}

int main()
{
  // Memoize free function
  auto memfoo = Memoized{foo};
  auto a      = memfoo(10, 5);
  auto b      = memfoo(10, 5);
  auto c      = memfoo(20, 0);

  return 0;
}
