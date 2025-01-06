#pragma once

#include "definitions.hpp"
#include <functional>
#include <queue>
#include <vector>
#include <string>

typedef std::vector<int> HeuristicOrder;

struct Heuristic {
  static bool isHeuristic(std::string id);

  static HeuristicOrder fromId(std::string id, const Graph &graph);
  static HeuristicOrder minDegreeFirst(const Graph &graph);
  static HeuristicOrder maxDegreeFirst(const Graph &graph);
};
