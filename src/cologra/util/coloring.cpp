#include "coloring.hpp"
#include <boost/graph/adjacency_list.hpp>

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

ColorMap getEmptyColorMap(Graph graph){
  std::vector<ColorType> colorVec(boost::num_vertices(graph));
  ColorMap coloring(&colorVec.front(), boost::get(vertex_index, graph));
  return coloring;
}
