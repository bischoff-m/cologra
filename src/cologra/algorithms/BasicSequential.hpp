#pragma once

#include "../ColoringAlgorithm.hpp"
#include "../definitions.hpp"

#include <queue>
#include <functional>

/**
 * Most basic implementation of the sequential graph coloring algorithm.
 *
 * #### Reference:
 *
 * Page 691 of the paper:
 * Gebremedhin et al. “What Color Is Your Jacobian? Graph Coloring
 * for Computing Derivatives.” SIAM Review 47, no. 4 (January 2005): 629–705.
 * https://doi.org/10.1137/S0036144504444711.
 */
class BasicSequential : public ColoringAlgorithm {
public:
  BasicSequential();
  OutType computeColoring(Graph graph) override;

  VerticesSizeType greedyColorOrdered(Graph graph, ColorMap coloring, std::priority_queue<int, std::vector<int>, std::function<bool(int, int)>> order);
};
