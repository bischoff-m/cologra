#pragma once

#include "../definitions.hpp"

/**
 * Check if the given coloring is a valid distance-1 coloring.
 * A coloring is distance-1 if no two adjacent nodes have the same color.
 *
 * @param graph Graph
 * @param coloring Coloring
 * @return True if the coloring is a valid distance-1 coloring
 */
bool isDistance1Coloring(Graph graph, ColorMap coloring);
ColorMap getEmptyColorMap(Graph graph);