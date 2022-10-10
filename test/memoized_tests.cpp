#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mnesic/memoized.hpp"

int foo_call_count = 0;
int foo(int a, int b) {
  foo_call_count++;
  return a + b;
}

TEST_CASE("memoized free function", "[free]") {
  foo_call_count = 0;
  auto mem_f = Memoized{foo};

  SECTION("the memoization can't have negative capacity", "[free]") {
    REQUIRE_THROWS(Memoized{foo, -1});
  }

  auto a = GENERATE(1, 2, 3, 4);
  SECTION("the memoized function produces identical result", "[free]") {
    auto b = GENERATE(5, 6, 7);
    REQUIRE(foo(a, b) == mem_f(a, b));
    REQUIRE(mem_f(a, b) == mem_f(a, b));
  }

  SECTION("the memoized function has the correct entry count", "[free]") {
    mem_f(a, 1);
    mem_f(a, 2);
    mem_f(a, 1);
    mem_f(a, 1);
    REQUIRE(2 == mem_f.record_count());
    REQUIRE(2 == foo_call_count);
  }

  SECTION("the memoized function has the correct capacity", "[free]") {
    auto c = GENERATE(10, 50, 128);
    auto m = Memoized{foo, c};
    REQUIRE(c == m.capacity());
  }

  SECTION("the entry count doesn't go above capacity", "[free]") {
    Memoized mem_f{foo, 1};
    mem_f(0, 0);
    mem_f(1, 1);
    mem_f(2, 2);
    REQUIRE(1 == mem_f.record_count());
    REQUIRE(3 == foo_call_count);
  }
}
