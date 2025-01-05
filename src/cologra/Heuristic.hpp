#pragma once

#include "definitions.hpp"
#include <functional>
#include <queue>
#include <string>

typedef std::
    priority_queue<int, std::vector<int>, std::function<bool(int, int)>>
        HeuristicQueue;

struct Heuristic {
  static HeuristicQueue minDegreeFirst(Graph graph);
  static HeuristicQueue maxDegreeFirst(Graph graph);
  static HeuristicQueue fromId(std::string id, Graph graph);
  static bool isHeuristic(std::string id);
};
