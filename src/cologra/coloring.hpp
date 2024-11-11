#pragma once

#include "definitions.hpp"

/**
 * Greedy sequential coloring
 *
 * #### Reference:
 *
 * Page 691 of the paper:
 * Gebremedhin et al. “What Color Is Your Jacobian? Graph Coloring
 * for Computing Derivatives.” SIAM Review 47, no. 4 (January 2005): 629–705.
 * https://doi.org/10.1137/S0036144504444711.
 *
 * @param graph Graph to color
 * @param coloring This map will be filled with the coloring
 * @return Number of colors used
 */
VerticesSizeType greedyColoring(Graph graph, ColorMap coloring);
