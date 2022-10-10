#include <iostream>
#include <utility>

#include "mnesic/memoized.hpp"

struct MyFunctor {
  std::pair<int, int> operator()(const std::string &str) const {
    call_count++;
    return std::make_pair(str.at(0), str.at(1));
  }
  int mutable call_count{0};
};

int main() {
  // Memoize any functor
  auto my_functor = MyFunctor{};
  auto memoized_functor = Memoized{my_functor};
  std::cout << memoized_functor("abc").first << ", "
            << memoized_functor("abc").second << "\n";
  std::cout << memoized_functor("efg").first << ", "
            << memoized_functor("efg").second << "\n";
  std::cout << "call_count " << my_functor.call_count << "\n";

  return 0;
}
