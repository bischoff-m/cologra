#pragma once

#include "../ColoringAlgorithm.hpp"
#include "../definitions.hpp"

using namespace cologra;

/**
 * Sequential vertex coloring algorithm from the Boost Graph Library. This is
 * not parallelized in any way.
 *
 * #### Reference:
 *
 * https://www.boost.org/doc/libs/1_85_0/libs/graph/doc/sequential_vertex_coloring.html
 */
class BoostSequential : public ColoringAlgorithm {
public:
  BoostSequential();
  Coloring computeColoring(Graph graph) override;
};
