file(GLOB_RECURSE cologra_sources "cologra/*.cpp")
file(GLOB_RECURSE cologra_headers "cologra/*.hpp")

file(GLOB_RECURSE benchmark_sources "benchmark/*.cpp")
file(GLOB_RECURSE benchmark_headers "benchmark/*.hpp")

add_library(cologra STATIC
    ${cologra_sources}
    ${cologra_headers}
)
add_library(benchmark STATIC
    ${benchmark_sources}
    ${benchmark_headers}
)

target_link_libraries(cologra PRIVATE fmt::fmt)
target_link_libraries(cologra PRIVATE MPI::MPI_CXX)
target_link_libraries(cologra PRIVATE Eigen3::Eigen)
target_link_libraries(cologra PUBLIC fast_matrix_market::fast_matrix_market)
target_link_libraries(cologra INTERFACE Boost::graph)
target_link_libraries(cologra PRIVATE Boost::mpi)
target_link_libraries(cologra PRIVATE Boost::serialization)

target_link_libraries(benchmark PRIVATE fmt::fmt)
target_link_libraries(benchmark PRIVATE MPI::MPI_CXX)
target_link_libraries(benchmark PRIVATE Eigen3::Eigen)
target_link_libraries(benchmark PUBLIC fast_matrix_market::fast_matrix_market)
target_link_libraries(benchmark INTERFACE Boost::graph)
target_link_libraries(benchmark PRIVATE Boost::stacktrace_basic)
target_link_libraries(benchmark PRIVATE lfreist-hwinfo::hwinfo)
target_link_libraries(benchmark PRIVATE cologra)
target_link_libraries(benchmark PRIVATE Boost::mpi)
target_link_libraries(benchmark PRIVATE Boost::serialization)

install(
    TARGETS cologra
    EXPORT colograTargets
    ARCHIVE DESTINATION lib/cologra
    LIBRARY DESTINATION lib/cologra
    INCLUDES DESTINATION include/cologra
)

add_executable(cologra_cli cli.cpp)
target_link_libraries(cologra_cli PUBLIC cologra)
target_link_libraries(cologra_cli PUBLIC benchmark)

install(TARGETS cologra_cli DESTINATION bin/cologra)
