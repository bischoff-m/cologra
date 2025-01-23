#include "OrderedSequential.hpp"
#include "../util/coloring.hpp"
#include <functional>
#include <iostream>
#include <queue>

using namespace std;

Vertex greedyColorOrdered(
    Graph graph, ColorIterator colorIter, HeuristicOrder order) {
  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(colorIter, node, -1);

  int current_node = 0;

  // Iterate over all nodes
  Vertex numColors = 0;
  while (current_node < order.size()) {
    auto node = order.at(current_node);
    current_node++;
    // Get the neighbors of the node
    auto neighbors = boost::adjacent_vertices(node.node, graph);

    // Get the colors of the neighbors
    vector<Vertex> neighborColors;
    for (auto neighbor : boost::make_iterator_range(neighbors))
      neighborColors.push_back(get(colorIter, neighbor));

    // Find the smallest color not used by any of its neighbors
    Vertex color;
    for (color = 0; color < boost::num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    boost::put(colorIter, node.node, color);
    numColors = max(numColors, color + 1);
  }
  return numColors;
}

OrderedSequential::OrderedSequential(const nlohmann::json &params)
    : ColoringAlgorithm(params, AlgorithmId("OrderedSequential", "1.0")) {
  if (!params.contains("heuristic"))
    throw invalid_argument("Missing heuristic parameter");
  if (!Heuristic::isHeuristic(params["heuristic"]))
    throw invalid_argument("Invalid heuristic parameter");
  heuristicId = params["heuristic"];
}

Coloring OrderedSequential::computeColoring(Graph graph) {
  auto heuristic = Heuristic::fromId(heuristicId, graph);
  auto [colorVec, colorIter] = getEmptyColoring(graph);
  Color numColors = greedyColorOrdered(graph, *colorIter, heuristic);
  return {numColors, std::move(colorVec)};
}
