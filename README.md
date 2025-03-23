# Mnesic

Dependency free, single header, C++17 [memoization](https://en.wikipedia.org/wiki/Memoization) class. Basically just a [kata](http://codekata.com/).

### Quick start

The project uses [xmake](https://xmake.io/#/?id=build-a-project) to both build and manage development dependencies.

```bash
git clone git@github.com:TheCoconutChef/mnesic.git
cd mnesic
xmake
```

#### Memoize a free function

```c++
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
```
Output:
```
Foo is called with 10, 5
Foo is called with 20, 0
Result: 50
```

#### Memoize a recursive function

```c++
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
// But something like this can also be done:
// https://stackoverflow.com/a/47457552

int main()
{
  auto fib = MemoizedFib();
  for (auto n = 20LL; n <= 30LL; ++n)
  {
    fmt::println("MEM_F_{} = {}", n, fib(n));
  }
  fmt::println("mem_fib_call_count: {}", fib.call_count);

  fmt::println("NAIVE_F_10 = {}", fib_naive(10));
  fmt::println("naive_fib_call_count: {}", naive_fib_call_count);

  return 0;
}
```
Output:
```
MEM_F_20 = 6765
MEM_F_21 = 10946
MEM_F_22 = 17711
MEM_F_23 = 28657
MEM_F_24 = 46368
MEM_F_25 = 75025
MEM_F_26 = 121393
MEM_F_27 = 196418
MEM_F_28 = 317811
MEM_F_29 = 514229
MEM_F_30 = 832040
mem_fib_call_count: 41
NAIVE_F_10 = 55
naive_fib_call_count: 177
```

#### Memoize a lambda

```c++
#include "mnesic/memoized.hpp"
#include <fmt/base.h>

int main()
{
  // Memoize a lambda
  int call_count = 0;
  auto f         = [&call_count](const std::string &user) -> std::string
  {
    call_count++;
    return "Hello " + user + "!";
  };
  auto f_mem = Memoized{f};
  fmt::println("{}", f_mem(std::string("Johnny")));
  fmt::println("{}", f_mem(std::string("Stacey")));
  fmt::println("{}", f_mem(std::string("Johnny")));
  fmt::println("call_count {}", call_count);

  return 0;
}
```
Output:
```
Hello Johnny!
Hello Stacey!
Hello Johnny!
call_count 2
```

#### Limitations

- Still not dealing with overload set.
- The cache policy is fixed, i.e. lru cache.
- The hash strategy is fixed.
- Not thread safe.
