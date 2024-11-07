cmake_minimum_required(VERSION 3.22)
project(cologra CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# -fopenmp flag (enables OpenMP)
# set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -funroll-loops -fopenmp")

add_executable(${PROJECT_NAME} src/main.cpp)

# MPI
find_package(MPI REQUIRED)
target_link_libraries(cologra PRIVATE MPI::MPI_CXX)

# Eigen3
find_package(Eigen3 CONFIG REQUIRED)
target_link_libraries(cologra PRIVATE Eigen3::Eigen)

# Fast Matrix Market
include(FetchContent)
FetchContent_Declare(
        fast_matrix_market
        GIT_REPOSITORY https://github.com/alugowski/fast_matrix_market
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fast_matrix_market)

target_link_libraries(cologra PRIVATE fast_matrix_market::fast_matrix_market)
