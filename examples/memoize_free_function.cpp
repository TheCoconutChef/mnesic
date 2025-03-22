#include <iostream>

#include "mnesic/memoized.hpp"

int foo(int a, int b)
{
  std::cout << "Foo is called with " << a << ", " << b << "\n";
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
