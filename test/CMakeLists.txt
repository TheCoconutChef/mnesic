add_executable(mnesic_test)
target_sources(mnesic_test PRIVATE memoized_tests.cpp)
target_link_libraries(mnesic_test PRIVATE mnesic Catch2::Catch2WithMain)
