#pragma once
#include "../definitions.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <queue>

typedef iterator_property_map<bool *, VertexIndexMap> TouchedMap;

/// @brief compute the degeneracy-graph of order k of the given graph - so just
/// throw out all nodes with degree strictly less than k
/// @param graph
/// @param k lowest degree to be kept in the graph
void degeneracy_graph(Graph &graph, DegreeType k) {
  std::priority_queue<VertexType> todo_list = {};
  for (VertexType node : boost::make_iterator_range(boost::vertices(graph))) {
    todo_list.push(node);
  }
  while (!todo_list.empty()) {
    VertexType node = todo_list.top();
    todo_list.pop();
    if (boost::degree(node, graph) < k) {
      boost::remove_vertex(node, graph);
      auto neighbors = boost::adjacent_vertices(node, graph);
      for (VertexType neighbor : boost::make_iterator_range(neighbors)) {
        todo_list.push(neighbor);
      }
    }
  }
}