#include "BasicParallel.hpp"
#include "../Heuristic.hpp"
#include "../util/coloring.hpp"
#include <Eigen/SparseCore>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/permutation_iterator.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using namespace std;

BasicParallel::BasicParallel()
    : ColoringAlgorithm(json(), AlgorithmId("BasicParallel", "1.0")) {}

ColorType sequenceDependentGreedyColoring(
    Graph graph, ColorMap coloring, VertexIteratorRange indices) {
  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(coloring, node, -1);

  // Iterate over all nodes in the given indices sequence
  ColorType numColors = 0;
  for (auto node : boost::make_iterator_range(
           boost::make_permutation_iterator(
               boost::vertices(graph).first, indices.first),
           boost::make_permutation_iterator(
               boost::vertices(graph).first, indices.second))) {
    // Get the neighbors of the node
    auto neighbors = boost::adjacent_vertices(node, graph);

    // Get the colors of the neighbors
    vector<ColorType> neighborColors;
    for (auto neighbor : boost::make_iterator_range(neighbors))
      neighborColors.push_back(coloring[neighbor]);

    // Find the smallest color not used by any of its neighbors
    ColorType color;
    for (color = 0; color < boost::num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    boost::put(coloring, node, color);
    numColors = max(numColors, color + 1);
  }
  return numColors;
}

ColorType sequenceDependentGreedyDist2Coloring(
    Graph graph, ColorMap coloring, VertexIteratorRange indices) {
  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(coloring, node, -1);

  // Iterate over all nodes in the given indices sequence
  ColorType numColors = 0;
  for (auto node : boost::make_iterator_range(
           boost::make_permutation_iterator(
               boost::vertices(graph).first, indices.first),
           boost::make_permutation_iterator(
               boost::vertices(graph).first, indices.second))) {
    // Get the neighbors of the node
    auto neighbors = boost::adjacent_vertices(node, graph);
    std::vector<VertexType> neighborsneighbors = {};
    for (auto neighbor : boost::make_iterator_range(neighbors)) {
      neighborsneighbors.push_back(neighbor);
      for (auto nn : boost::make_iterator_range(
               boost::adjacent_vertices(neighbor, graph))) {
        neighborsneighbors.push_back(nn);
      }
    }

    // Get the colors of the neighbors
    vector<ColorType> neighborColors;
    for (auto neighbor : neighborsneighbors)
      neighborColors.push_back(coloring[neighbor]);

    // Find the smallest color not used by any of its neighbors
    ColorType color;
    for (color = 0; color < boost::num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    boost::put(coloring, node, color);
    numColors = max(numColors, color + 1);
  }
  return numColors;
}

OutType BasicParallel::computeColoring(Graph graph) {
  ColorMap coloring = getEmptyColorMap(graph);
  VertexIteratorRange identity_ordering = boost::vertices(graph);

  std::vector<VertexIteratorRange> index_iterators = {identity_ordering};
  ColorType numColors = std::numeric_limits<ColorType>::max();
#pragma omp parallel for reduction(min : numColors)
  for (auto &indices : index_iterators) {
    ColorMap local_coloring = getEmptyColorMap(graph);
    ColorType local_numColors =
        sequenceDependentGreedyColoring(graph, local_coloring, indices);
#pragma omp critical
    {
      if (local_numColors < numColors) {
        coloring = local_coloring;
        numColors = local_numColors;
      }
    }
  }
  return {numColors, coloring};
}

OutType BasicParallel::computeDist2Coloring(Graph graph) {
  ColorMap coloring = getEmptyColorMap(graph);
  VertexIteratorRange identity_ordering = boost::vertices(graph);

  std::vector<VertexIteratorRange> index_iterators = {identity_ordering};
  ColorType numColors = std::numeric_limits<ColorType>::max();
#pragma omp parallel for reduction(min : numColors)
  for (auto &indices : index_iterators) {
    ColorMap local_coloring = getEmptyColorMap(graph);
    ColorType local_numColors =
        sequenceDependentGreedyDist2Coloring(graph, local_coloring, indices);
#pragma omp critical
    {
      if (local_numColors < numColors) {
        coloring = local_coloring;
        numColors = local_numColors;
      }
    }
  }
  return {numColors, coloring};
}
