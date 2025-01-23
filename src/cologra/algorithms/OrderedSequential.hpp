#pragma once

#include "../ColoringAlgorithm.hpp"
#include "../Heuristic.hpp"
#include "../definitions.hpp"

using namespace cologra;

/**
 * Uses a heuristic to order the nodes and then colors them in that order.
 */
class OrderedSequential : public ColoringAlgorithm {
private:
  string heuristicId;

public:
  OrderedSequential(const nlohmann::json &params);
  Coloring computeColoring(Graph graph) override;
};
