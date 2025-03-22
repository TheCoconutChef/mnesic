#include "mnesic/memoized.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <string>
#include <utility>

struct MyFunctor
{
  std::pair<int, int> operator()(const std::string &str)
  {
    call_count++;
    return std::make_pair(str.at(0), str.at(1));
  }
  int call_count{0};
};

int main()
{
  // Memoize any functor
  auto my_functor       = MyFunctor();
  auto memoized_functor = Memoized(my_functor);
  fmt::println("{}, {}", memoized_functor(std::string("abc")), memoized_functor(std::string("abc")));
  fmt::println("{}, {}", memoized_functor(std::string("efg")).first, memoized_functor(std::string("efg")).second);
  fmt::println("call_count {}", my_functor.call_count);

  return 0;
}
