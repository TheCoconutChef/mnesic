cmake_minimum_required(VERSION 3.13)
project(mnesic)
add_compile_options(-std=c++17)

add_library(mnesic INTERFACE)
target_sources(mnesic INTERFACE mnesic/memoized.hpp)
target_include_directories(mnesic INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CPM_USE_LOCAL_PACKAGES ON)

if (BUILD_TESTING)
  include(cmake/CPM.cmake)
  include(cmake/catch2.cmake)
  add_subdirectory(test)
endif()

option(BUILD_EXAMPLES "Build examples in the example/ folder.")
if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
