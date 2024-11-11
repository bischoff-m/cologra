file(GLOB_RECURSE cologra_sources "cologra/*.cpp")
file(GLOB_RECURSE cologra_headers "cologra/*.hpp")

add_library(cologra STATIC
    ${cologra_sources}
    ${cologra_headers}
)
target_link_libraries(cologra PRIVATE fmt::fmt)
target_link_libraries(cologra PRIVATE MPI::MPI_CXX)
target_link_libraries(cologra PRIVATE Eigen3::Eigen)
target_link_libraries(cologra PUBLIC fast_matrix_market::fast_matrix_market)
target_link_libraries(cologra INTERFACE Boost::graph)

install(
    TARGETS cologra
    EXPORT colograTargets
    ARCHIVE DESTINATION lib/cologra
    LIBRARY DESTINATION lib/cologra
    INCLUDES DESTINATION include/cologra
)

add_executable(cologra_cli main.cpp)
target_link_libraries(cologra_cli PUBLIC cologra)

install(TARGETS cologra_cli DESTINATION bin/cologra)
