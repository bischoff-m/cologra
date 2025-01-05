#include "OrderedSequential.hpp"
#include <functional>
#include <iostream>
#include <queue>

using namespace std;

VerticesSizeType greedyColorOrdered(
    Graph graph, ColorMap coloring, HeuristicQueue order) {
  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(coloring, node, -1);

  // Iterate over all nodes
  VerticesSizeType numColors = 0;
  while (!order.empty()) {
    auto node = order.top();
    order.pop();
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

OrderedSequential::OrderedSequential(const nlohmann::json &params)
    : ColoringAlgorithm(params, AlgorithmId("OrderedSequential", "1.0")) {
  if (!params.contains("heuristic"))
    throw invalid_argument("Missing heuristic parameter");
  if (!Heuristic::isHeuristic(params["heuristic"]))
    throw invalid_argument("Invalid heuristic parameter");
  heuristicId = params["heuristic"];
}

VerticesSizeType OrderedSequential::computeColoring(
    Graph graph, ColorMap coloring) {
  auto heuristic = Heuristic::fromId(heuristicId, graph);
  return greedyColorOrdered(graph, coloring, heuristic);
}
