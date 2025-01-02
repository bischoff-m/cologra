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
ColorType greedyColoring(Graph graph, ColorMap coloring);
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
 * @param indices This Map contains the sequence in which to color the nodes
 * @return Number of colors used
 */
ColorType sequenceDependentGreedyColoring(Graph graph, ColorMap coloring, VertexIndexMap indices);

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
 * this is parallelized over n cores with openmp
 *
 * @param graph Graph to color
 * @param coloring This map will be filled with the coloring which uses the minimum number of colors
 * @param index_iterators This vector of maps contains the sequences according to which to color the nodes
 * @return Number of colors used in that minimum
 */
ColorType parallelGreedyColoring(Graph graph, ColorMap coloring, std::vector<VertexIndexMap> index_iterators);

/**
 * Check if the given coloring is a valid distance-1 coloring.
 * A coloring is distance-1 if no two adjacent nodes have the same color.
 *
 * @param graph Graph
 * @param coloring Coloring
 * @return True if the coloring is a valid distance-1 coloring
 */
bool isDistance1Coloring(Graph graph, ColorMap coloring);
