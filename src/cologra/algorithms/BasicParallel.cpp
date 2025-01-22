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
    Graph graph, ColorVector colorVec, VertexIteratorRange indices) {
  auto colorIter = getEmptyColoring(graph, colorVec);
  // Set all entries in the coloring to -1
  for (auto node : make_iterator_range(vertices(graph)))
    put(*colorIter, node, -1);

  // Iterate over all nodes in the given indices sequence
  ColorType numColors = 0;
  for (auto node : make_iterator_range(
           make_permutation_iterator(vertices(graph).first, indices.first),
           make_permutation_iterator(vertices(graph).first, indices.second))) {
    // Get the neighbors of the node
    auto neighbors = adjacent_vertices(node, graph);

    // Get the colors of the neighbors
    vector<ColorType> neighborColors;
    for (auto neighbor : make_iterator_range(neighbors))
      neighborColors.push_back(get(*colorIter, neighbor));

    // Find the smallest color not used by any of its neighbors
    ColorType color;
    for (color = 0; color < num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    put(*colorIter, node, color);
    numColors = max(numColors, color + 1);
  }
  return numColors;
}

ColorType sequenceDependentGreedyDist2Coloring(
    Graph graph, ColorVector colorVec, VertexIteratorRange indices) {
  auto colorIter = getEmptyColoring(graph, colorVec);
  // Set all entries in the coloring to -1
  for (auto node : make_iterator_range(vertices(graph)))
    put(*colorIter, node, -1);

  // Iterate over all nodes in the given indices sequence
  ColorType numColors = 0;
  for (auto node : make_iterator_range(
           make_permutation_iterator(vertices(graph).first, indices.first),
           make_permutation_iterator(vertices(graph).first, indices.second))) {
    // Get the neighbors of the node
    auto neighbors = adjacent_vertices(node, graph);
    std::vector<VertexType> neighborsNeighbors = {};
    for (auto neighbor : make_iterator_range(neighbors)) {
      neighborsNeighbors.push_back(neighbor);
      for (auto nn : make_iterator_range(adjacent_vertices(neighbor, graph))) {
        neighborsNeighbors.push_back(nn);
      }
    }

    // Get the colors of the neighbors
    vector<ColorType> neighborColors;
    for (auto neighbor : neighborsNeighbors)
      neighborColors.push_back(get(*colorIter, neighbor));

    // Find the smallest color not used by any of its neighbors
    ColorType color;
    for (color = 0; color < num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    put(*colorIter, node, color);
    numColors = max(numColors, color + 1);
  }
  return numColors;
}

OutType computeColoringGeneral(Graph graph,
    function<ColorType(
        Graph graph, ColorVector colorVec, VertexIteratorRange indices)>
        coloringFunction) {
  unique_ptr<ColorVector> colorVec =
      make_unique<ColorVector>(num_vertices(graph));
  VertexIteratorRange identityOrdering = vertices(graph);

  std::vector<VertexIteratorRange> indexIterators = {identityOrdering};
  ColorType numColors = std::numeric_limits<ColorType>::max();
#pragma omp parallel for reduction(min : numColors)
  for (auto &indices : indexIterators) {
    auto localColorVec = make_unique<ColorVector>(num_vertices(graph));
    ColorType localNumColors = coloringFunction(graph, *localColorVec, indices);
#pragma omp critical
    {
      if (localNumColors < numColors) {
        colorVec = std::move(localColorVec);
        numColors = localNumColors;
      }
    }
  }
  return {numColors, std::move(colorVec)};
}

OutType BasicParallel::computeColoring(Graph graph) {
  return computeColoringGeneral(graph, sequenceDependentGreedyColoring);
}

OutType BasicParallel::computeDist2Coloring(Graph graph) {
  return computeColoringGeneral(graph, sequenceDependentGreedyDist2Coloring);
}
