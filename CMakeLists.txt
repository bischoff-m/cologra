cmake_minimum_required(VERSION 3.28)
project(cologra CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -funroll-loops -fopenmp")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
#set(OpenMP_CXX_FLAGS "${OpenMP_CXX_FLAGS} -fopenmp")
#set(OpenMP_CXX_LIB_NAMES "${OpenMP_CXX_LIB_NAMES}" "")

# -fopenmp flag (enables OpenMP)
# set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -funroll-loops -fopenmp")
# find_package(OpenMP REQUIRED)
#target_link_libraries(cologra PUBLIC OpenMP::OpenMP_CXX)

# fmt
find_package(fmt CONFIG REQUIRED)

# MPI
find_package(MPI REQUIRED)
#target_link_libraries(cologra PUBLIC MPI::MPI_CXX)
# Eigen3
find_package(Eigen3 CONFIG REQUIRED)

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
