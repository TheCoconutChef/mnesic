#include <iostream>

#include "mnesic/memoized.hpp"

int naive_fib_call_count = 0;
long long int fib_naive(long long int n) {
  naive_fib_call_count++;
  return n <= 1 ? n : fib_naive(n - 1) + fib_naive(n - 2);
}

int fib_call_count = 0;
long long int fib(long long int n);

auto memoized = Memoized{fib};
long long int fib(long long int n) {
  fib_call_count++;
  return n <= 1 ? n : memoized(n - 1LL) + memoized(n - 2LL);
}

int main() {
  // Call to recursively defined free function
  for (auto n = 20LL; n <= 30LL; ++n)
    std::cout << "F_" << n << " = " << fib(n) << "\n";
  std::cout << "F_10 = " << fib_naive(10) << "\n";
  std::cout << "mem_fib_call_count: " << fib_call_count << "\n";
  std::cout << "naive_fib_call_count: " << naive_fib_call_count << "\n";

  return 0;
}
