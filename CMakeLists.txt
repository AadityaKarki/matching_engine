cmake_minimum_required(VERSION 3.14)
project(MatchingEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---------------------------------------------------------------------------
# Header-only engine library
# ---------------------------------------------------------------------------
add_library(matching_engine INTERFACE)
target_include_directories(matching_engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/engine)

# ---------------------------------------------------------------------------
# Demo executable
# ---------------------------------------------------------------------------
add_executable(main main.cpp)
target_link_libraries(main PRIVATE matching_engine)

# ---------------------------------------------------------------------------
# Tests — Catch2 (fetched automatically)
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

add_executable(tests tests/test_order_book.cpp)
target_link_libraries(tests PRIVATE matching_engine Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(tests)
