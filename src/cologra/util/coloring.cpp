#include "coloring.hpp"
#include <boost/graph/adjacency_list.hpp>

using namespace std;

bool isDistance1Coloring(Graph graph, ColorVector &colorVec) {
  auto colorIter = getEmptyColoring(graph, colorVec);
  for (auto node : make_iterator_range(vertices(graph))) {
    auto neighbors = adjacent_vertices(node, graph);
    for (auto neighbor : make_iterator_range(neighbors)) {
      // Skip self-loops
      if (node == neighbor)
        continue;
      // Check if the node and its neighbor have the same color
      if (get(*colorIter, node) == get(*colorIter, neighbor))
        return false;
    }
  }
  return true;
}

pair<unique_ptr<ColorVector>, unique_ptr<ColorIterator>> getEmptyColoring(
    Graph graph) {
  auto colorVec = make_unique<vector<ColorType>>(num_vertices(graph));
  auto colorIter = getEmptyColoring(graph, *colorVec);
  return {std::move(colorVec), std::move(colorIter)};
}

unique_ptr<ColorIterator> getEmptyColoring(Graph graph, ColorVector &colorVec) {
  auto colorIter =
      make_unique<ColorIterator>(&colorVec.front(), get(vertex_index, graph));
  return std::move(colorIter);
}
