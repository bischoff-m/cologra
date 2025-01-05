#include "Heuristic.hpp"

HeuristicQueue Heuristic::fromId(std::string id, Graph graph) {
  if (id == "minDegreeFirst") {
    return minDegreeFirst(graph);
  } else if (id == "maxDegreeFirst") {
    return maxDegreeFirst(graph);
  } else {
    throw std::invalid_argument("Invalid heuristic id");
  }
};

bool Heuristic::isHeuristic(std::string id) {
  return id == "minDegreeFirst" || id == "maxDegreeFirst";
};

HeuristicQueue Heuristic::minDegreeFirst(Graph graph) {
  auto min_degree = [&graph](int a, int b) {
    return degree(a, graph) > degree(b, graph);
  };

  HeuristicQueue q(min_degree);
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    q.emplace(node);
  }
  return q;
};

HeuristicQueue Heuristic::maxDegreeFirst(Graph graph) {
  auto max_degree = [&graph](int a, int b) {
    return degree(a, graph) < degree(b, graph);
  };

  HeuristicQueue q(max_degree);
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    q.emplace(node);
  }
  return q;
};
