cmake_minimum_required(VERSION 3.26)
project(cologra CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt CONFIG REQUIRED)
find_package(boost_mpi CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(boost_serialization CONFIG REQUIRED)

# Fast Matrix Market
include(FetchContent)
FetchContent_Declare(
        fast_matrix_market
        GIT_REPOSITORY https://github.com/alugowski/fast_matrix_market
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fast_matrix_market)

# Boost Library
find_package(boost_graph REQUIRED COMPONENTS)
find_package(boost_stacktrace_basic CONFIG REQUIRED)

# Hwinfo
find_package(lfreist-hwinfo REQUIRED)

# Source files
add_subdirectory(src)

# Google Test
add_subdirectory(tests)
