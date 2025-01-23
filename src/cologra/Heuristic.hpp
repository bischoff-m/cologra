#pragma once

#include "definitions.hpp"
#include <functional>
#include <queue>
#include <string>
#include <vector>

using namespace std;
using namespace cologra;

struct HeuristicNodePair {
  Vertex node;
  int value;
};

/**
 * An order of vertices. First element is the vertex, second element is the
 * heuristic value.
 */
typedef vector<HeuristicNodePair> HeuristicOrder;

struct Heuristic {
  static bool isHeuristic(std::string id);

  static HeuristicOrder fromId(std::string id, const Graph &graph);
  static HeuristicOrder minDegreeFirst(const Graph &graph);
  static HeuristicOrder maxDegreeFirst(const Graph &graph);
};
