#pragma once

#include "../definitions.hpp"
#include <filesystem>

/**
 * Save a graph to a DOT (Graphviz) file with the given coloring.
 *
 * @param outfile Path to write the DOT file
 * @param graph Graph to save
 * @param coloring Coloring of the graph
 */
void graphToDot(std::filesystem::path outfile, Graph graph, ColorVector colorVec);
