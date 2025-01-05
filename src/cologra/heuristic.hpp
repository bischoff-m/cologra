#pragma once

#include "definitions.hpp"
#include <queue>
#include <functional>

namespace heuristic {

auto min_degree_first(Graph graph) {
  auto min_degree = [&graph](VertexType a, VertexType b) {
    return degree(a, graph) > degree(b, graph);
  };

  std::priority_queue<DegreeType, std::vector<VertexType>, std::function<bool(VertexType, VertexType)>> q(
      min_degree);
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    q.emplace(node);
  }
  return q;
};

auto max_degree_first(Graph graph) {
  auto max_degree = [&graph](VertexType a, VertexType b) {
    return degree(a, graph) < degree(b, graph);
  };

  std::priority_queue<DegreeType, std::vector<VertexType>, std::function<bool(VertexType, VertexType)>> q(
      max_degree);
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    q.emplace(node);
  }
  return q;
};

}; // namespace heuristic