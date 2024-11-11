#include "definitions.hpp"
#include <Eigen/SparseCore>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <fast_matrix_market/app/Eigen.hpp>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

VerticesSizeType greedyColoring(Graph graph, ColorMap coloring) {
  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(coloring, node, -1);

  // Iterate over all nodes
  VerticesSizeType numColors = 0;
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    // Get the neighbors of the node
    auto neighbors = boost::adjacent_vertices(node, graph);

    // Get the colors of the neighbors
    vector<VerticesSizeType> neighborColors;
    for (auto neighbor : boost::make_iterator_range(neighbors))
      neighborColors.push_back(coloring[neighbor]);

    // Find the smallest color not used by any of its neighbors
    VerticesSizeType color;
    for (color = 0; color < boost::num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    boost::put(coloring, node, color);
    numColors = max(numColors, color + 1);
  }
  return numColors;
}

bool isDistance1Coloring(Graph graph, ColorMap coloring) {
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    auto neighbors = boost::adjacent_vertices(node, graph);
    for (auto neighbor : boost::make_iterator_range(neighbors)) {
      // Skip self-loops
      if (node == neighbor)
        continue;
      // Check if the node and its neighbor have the same color
      if (boost::get(coloring, node) == boost::get(coloring, neighbor))
        return false;
    }
  }
  return true;
}
