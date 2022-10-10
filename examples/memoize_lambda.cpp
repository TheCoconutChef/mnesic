#include <iostream>

#include "mnesic/memoized.hpp"

int main() {
  // Memoize a lambda
  int call_count = 0;
  auto f = [&call_count](const std::string &user) {
    call_count++;
    return "Hello " + user + "!";
  };
  auto f_mem = Memoized{f};
  std::cout << f_mem("Johnny") << "\n";
  std::cout << f_mem("Stacey") << "\n";
  std::cout << f_mem("Johnny") << "\n";
  std::cout << "call_count " << call_count << "\n";

  return 0;
}
