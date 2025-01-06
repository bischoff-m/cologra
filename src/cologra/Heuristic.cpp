#include "Heuristic.hpp"

#include <algorithm>

HeuristicOrder Heuristic::fromId(std::string id, const Graph &graph) {
  if (id == "minDegreeFirst") {
    return minDegreeFirst(graph);
  } else if (id == "maxDegreeFirst") {
    return maxDegreeFirst(graph);
  } else {
    throw std::invalid_argument("Invalid heuristic id");
  }
};

HeuristicOrder Heuristic::minDegreeFirst(const Graph &graph) {
  std::vector<int> order;
  order.reserve(graph.m_vertices.size());
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
      order.push_back(node);
  }

  std::sort(order.begin(), order.end(), [&graph](int a, int b) {
    return degree(a, graph) < degree(b, graph);
  });

  return order;
}

HeuristicOrder Heuristic::maxDegreeFirst(const Graph &graph) {
  std::vector<int> order;
  order.reserve(graph.m_vertices.size());
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
      order.push_back(node);
  }

  std::sort(order.begin(), order.end(), [&graph](int a, int b) {
    return degree(a, graph) > degree(b, graph);
  });

  return order;
}


bool Heuristic::isHeuristic(std::string id) {
  return id == "minDegreeFirst" || id == "maxDegreeFirst";
};