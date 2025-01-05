#include "BasicSequential.hpp"
#include <Eigen/SparseCore>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <nlohmann/json.hpp>
#include <vector>

using namespace std;

BasicSequential::BasicSequential()
    : ColoringAlgorithm(
          nlohmann::json(), AlgorithmId("BasicSequential", "1.0")) {}

ColorType BasicSequential::computeColoring(
    Graph graph, ColorMap coloring) {
  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(coloring, node, -1);

  // Iterate over all nodes
  ColorType numColors = 0;
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
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

ColorType BasicSequential::greedyColorOrdered(Graph graph, ColorMap coloring, std::priority_queue<DegreeType, std::vector<VertexType>, std::function<bool(VertexType, VertexType)>> order) {
  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(coloring, node, -1);

  // Iterate over all nodes
  ColorType numColors = 0;
  while (!order.empty()) {
    auto node = order.top();
    order.pop();
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
