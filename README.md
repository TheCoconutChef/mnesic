# Mnesic

A feature poor but friendly [memoization](https://en.wikipedia.org/wiki/Memoization) class. Basically just a [kata](http://codekata.com/).

#### Limitations

- Still not dealing with overload set.
- The cache policy is fixed, i.e. lru cache.
- The hash strategy is fixed.
- Not thread safe.

### Quick start

The project uses [xmake](https://xmake.io/#/?id=build-a-project) to both build and manage dependencies.

```bash
git clone git@github.com:TheCoconutChef/mnesic.git
cd mnesic
xmake
```

#### Memoize a free function

```c++
int foo(int a, int b) {
  std::cout << "Foo is called with " << a << ", " << b << "\n";
  return a + b;
}

int main() {
  auto memfoo = Memoized{foo};

  auto a = memfoo(10, 5);
  auto b = memfoo(10, 5);
  auto c = memfoo(20, 0);

  std::cout << "Result: " << a + b + c << "\n";
}
```
Output:
```
Foo is called with 10, 5
Foo is called with 20, 0
Result: 50
```

#### Memoize a recursive free function

```c++
// Inspired by:
// https://stackoverflow.com/a/47457552
//

// in the header
int mem_fib_call_count = 0;
long long int fib(long long int n); 

// in the cpp
auto memoized = Memoized{fib};
long long int fib(long long int n) {
  mem_fib_call_count++:
  return n <= 1 ? n : memoized(n - 1LL) + memoized(n - 2LL);
}

int main() {
  for (auto n = 20LL; n <= 30LL; ++n)
    std::cout << "F_" << n << " = " << fib(n) << "\n";

  std::cout << "mem_fib_call_count: " << mem_fib_call_count << "\n";
  return 0;
}
```
Output:
```
F_20 = 6765
F_21 = 10946
F_22 = 17711
F_23 = 28657
F_24 = 46368
F_25 = 75025
F_26 = 121393
F_27 = 196418
F_28 = 317811
F_29 = 514229
F_30 = 832040
mem_fib_call_count: 41
```
Compared to a naive fib implementation:
```c++
int naive_fib_call_count = 0;
long long int fib_naive(long long int n) {
  naive_fib_call_count++;
  return n <= 1 ? n : fib_naive(n - 1) + fib_naive(n - 2);
}

int main() {
  std::cout << "F_10 = " << fib_naive(10) << "\n";
  std::cout << "naive_fib_call_count: " << "\n";
  return 0;
}
```
Output:
```
F_10 = 55
naive_fib_call_count: 177
```

#### Memoize a lambda

```c++
int main() {
  int call_count = 0;
  auto f = [&call_count](const std::string& user) { 
    call_count++;
    return "Hello " + user + "!"; 
  };
  auto f_mem = Memoized{f};

  std::cout << f_mem("Johnny") << "\n";
  std::cout << f_mem("Stacey") << "\n";
  std::cout << f_mem("Johnny") << "\n";

  std::cout << "call_count: " << call_count << "\n";

  return 0;
}
```
Output:
```
Hello Johnny!
Hello Stacey!
Hello Johnny!
call_count: 2
```

#### Memoize your own functor

```c++
struct MyFunctor {
  std::pair<int, int> operator()(const std::string &str) const {
    call_count++;
    return std::make_pair(str.at(0), str.at(1));
  }
  int mutable call_count{0};
};

inf main() {
  auto my_functor = MyFunctor{};
  auto memoized_functor = Memoized{my_functor};
  std::cout << memoized_functor("abc").first << ", "
            << memoized_functor("abc").second << "\n";
  std::cout << memoized_functor("efg").first << ", "
            << memoized_functor("efg").second << "\n";
  std::cout << "call_count: " << my_functor.call_count << "\n";
  return 0;
}
```
Output:
```
97, 98
101, 102
call_count: 2
```
