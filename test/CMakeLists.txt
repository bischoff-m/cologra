cmake_minimum_required(VERSION 3.22)
project(cologra-test CXX)

add_executable(${PROJECT_NAME} main.cpp)

# MPI
find_package(MPI REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE MPI::MPI_CXX)

# Eigen3
find_package(Eigen3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)

# Fast Matrix Market
include(FetchContent)
FetchContent_Declare(
        fast_matrix_market
        GIT_REPOSITORY https://github.com/alugowski/fast_matrix_market
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fast_matrix_market)
target_link_libraries(${PROJECT_NAME} PRIVATE fast_matrix_market::fast_matrix_market)

# Google Test
enable_testing()
find_package(GTest CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(AllTestsInMain ${PROJECT_NAME})
