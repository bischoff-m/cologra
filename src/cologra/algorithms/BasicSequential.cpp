#include "BasicSequential.hpp"
#include "../util/coloring.hpp"
#include <Eigen/SparseCore>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <nlohmann/json.hpp>
#include <vector>

using namespace std;

BasicSequential::BasicSequential()
    : ColoringAlgorithm(
          nlohmann::json(), AlgorithmId("BasicSequential", "1.0")) {}

Coloring BasicSequential::computeColoring(Graph graph) {
  auto [colorVec, colorIter] = getEmptyColoring(graph);
  // Set all entries in the coloring to -1
  for (auto node : make_iterator_range(vertices(graph)))
    put(*colorIter, node, -1);

  // Iterate over all nodes
  Vertex numColors = 0;
  for (auto node : make_iterator_range(vertices(graph))) {
    // Get the neighbors of the node
    auto neighbors = adjacent_vertices(node, graph);

    // Get the colors of the neighbors
    vector<Vertex> neighborColors;
    for (auto neighbor : make_iterator_range(neighbors))
      neighborColors.push_back(get(*colorIter, neighbor));

    // Find the smallest color not used by any of its neighbors
    Vertex color;
    for (color = 0; color < num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    put(*colorIter, node, color);
    numColors = max(numColors, color + 1);
  }
  return {numColors, std::move(colorVec)};
}
