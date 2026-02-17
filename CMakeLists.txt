cmake_minimum_required(VERSION 3.10)
project(MatchingEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create an interface library for the header-only engine
add_library(matching_engine INTERFACE)
target_include_directories(matching_engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/engine)

# Create an executable main
add_executable(main main.cpp)
target_link_libraries(main PRIVATE matching_engine)
