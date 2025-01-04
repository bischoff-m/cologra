#pragma once

#include "definitions.hpp"
#include <queue>
#include <functional>

namespace heuristic {

auto min_degree_first(Graph graph) {
  auto min_degree = [&graph](int a, int b) {
    return degree(a, graph) > degree(b, graph);
  };

  std::priority_queue<int, std::vector<int>, std::function<bool(int, int)>> q(
      min_degree);
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    q.emplace(node);
  }
  return q;
};

auto max_degree_first(Graph graph) {
  auto max_degree = [&graph](int a, int b) {
    return degree(a, graph) < degree(b, graph);
  };

  std::priority_queue<int, std::vector<int>, std::function<bool(int, int)>> q(
      max_degree);
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    q.emplace(node);
  }
  return q;
};

}; // namespace heuristic