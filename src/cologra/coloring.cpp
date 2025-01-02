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
#include <omp.h>

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

VerticesSizeType sequenceDependentGreedyColoring(Graph graph, ColorMap coloring, VertexIndexMap indices) {
  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(coloring, node, -1);

  // Iterate over all nodes in the given indices sequence
  VerticesSizeType numColors = 0;
  for (auto node : boost::make_permutation_iterator(boost::vertices(graph), indices)) {
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

VerticesSizeType parallelGreedyColoring(Graph graph, ColorMap coloring, std::vector<VertexIndexMap> index_iterators) {
  VerticesSizeType numColors = std::numerical_limits<VerticesSizeType>::max();
  #pragma omp parallel for reduction( min : numColors )
  for (auto& indices : index_iterators){
    ColorMap local_coloring;
    VerticesSizeType local_numColors = sequenceDependentGreedyColoring(Graph graph, ColorMap local_coloring, VertexIndexMap indices);
    #pragma omp critical
    {
      if (local_numColors<numColors){
        coloring = local_coloring;
        numColors = local_numColors;
      }
    }
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
