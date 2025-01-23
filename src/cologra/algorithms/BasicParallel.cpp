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

enum ColoringType { DIST1, DIST2 };

typedef function<ColorType(Graph, ColorVector, HeuristicOrder, ColoringType)>
    ColoringFunction;

ColorType coloringOrdered(Graph graph,
    ColorVector colorVec,
    HeuristicOrder order,
    ColoringType coloringType) {
  auto colorIter = getEmptyColoring(graph, colorVec);
  // Set all entries in the coloring to -1
  for (auto node : make_iterator_range(vertices(graph)))
    put(*colorIter, node, -1);

  // Iterate over all nodes in the given indices sequence
  ColorType numColors = 0;
  for (auto node : order) {
    // Get the colors of the neighbors
    vector<ColorType> neighborColors;
    // Get the neighbors of the node
    auto neighbors = adjacent_vertices(node.node, graph);

    if (coloringType == DIST1) {
      for (auto neighbor : make_iterator_range(neighbors))
        neighborColors.push_back(get(*colorIter, neighbor));
    } else {
      // Get the neighbors of the neighbors
      vector<VertexType> neighborsNeighbors = {};
      for (auto neighbor : make_iterator_range(neighbors)) {
        neighborsNeighbors.push_back(neighbor);
        for (auto nn :
            make_iterator_range(adjacent_vertices(neighbor, graph))) {
          neighborsNeighbors.push_back(nn);
        }
      }
      // Get the colors of the neighbors-neighbors
      for (auto neighbor : make_iterator_range(neighborsNeighbors))
        neighborColors.push_back(get(*colorIter, neighbor));
    }

    // Find the smallest color not used by any of its neighbors
    ColorType color;
    for (color = 0; color < num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    put(*colorIter, node.node, color);
    numColors = max(numColors, color + 1);
  }
  return numColors;
}

OutType computeColoringGeneral(Graph graph,
    ColoringFunction colFunc,
    vector<HeuristicOrder> orders,
    ColoringType coloringType) {

  unique_ptr<ColorVector> colorVec =
      make_unique<ColorVector>(num_vertices(graph));
  ColorType numColors = std::numeric_limits<ColorType>::max();
#pragma omp parallel for
  for (auto &order : orders) {
    auto localColorVec = make_unique<ColorVector>(num_vertices(graph));
    ColorType localNumColors =
        colFunc(graph, *localColorVec, order, coloringType);
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
  return computeColoringGeneral(graph,
      coloringOrdered,
      {Heuristic::minDegreeFirst(graph), Heuristic::maxDegreeFirst(graph)},
      ColoringType::DIST1);
}

OutType BasicParallel::computeDist2Coloring(Graph graph) {
  return computeColoringGeneral(graph,
      coloringOrdered,
      {Heuristic::minDegreeFirst(graph), Heuristic::maxDegreeFirst(graph)},
      ColoringType::DIST2);
}
