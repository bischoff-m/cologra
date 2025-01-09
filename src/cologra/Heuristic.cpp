#include "Heuristic.hpp"
#include <algorithm>

using namespace std;

HeuristicOrder Heuristic::fromId(string id, const Graph &graph) {
  if (id == "minDegreeFirst") {
    return minDegreeFirst(graph);
  } else if (id == "maxDegreeFirst") {
    return maxDegreeFirst(graph);
  } else {
    throw invalid_argument("Invalid heuristic id");
  }
};

HeuristicOrder Heuristic::minDegreeFirst(const Graph &graph) {
  // Create a vector of pairs (vertex, degree)
  HeuristicOrder order;
  order.reserve(num_vertices(graph));
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    HeuristicNodePair nodeAndValue;
    nodeAndValue.node = node;
    nodeAndValue.value = degree(node, graph);
    order.push_back(nodeAndValue);
  }
  // Sort by degree
  sort(order.begin(),
      order.end(),
      [](const HeuristicNodePair &a, const HeuristicNodePair &b) {
        return a.value < b.value;
      });
  return order;
}

HeuristicOrder Heuristic::maxDegreeFirst(const Graph &graph) {
  // Create a vector of pairs (vertex, degree)
  HeuristicOrder order;
  order.reserve(num_vertices(graph));
  for (auto node : boost::make_iterator_range(boost::vertices(graph))) {
    HeuristicNodePair nodeAndValue;
    nodeAndValue.node = node;
    nodeAndValue.value = degree(node, graph);
    order.push_back(nodeAndValue);
  }
  // Sort by degree
  sort(order.begin(),
      order.end(),
      [](const HeuristicNodePair &a, const HeuristicNodePair &b) {
        return a.value > b.value;
      });
  return order;
}

bool Heuristic::isHeuristic(string id) {
  return id == "minDegreeFirst" || id == "maxDegreeFirst";
};

template <class Archive>
void HeuristicNodePair::serialize(Archive &ar, const unsigned int version) {
  ar &node;
  ar &value;
}
