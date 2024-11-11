add_executable(cologra_tests main.cpp)

# Source files
target_include_directories(cologra_tests PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(cologra_tests PUBLIC cologra)

# Google Test
enable_testing()
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

target_link_libraries(cologra_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
gtest_discover_tests(cologra_tests)
