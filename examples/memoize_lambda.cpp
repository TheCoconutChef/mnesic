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
