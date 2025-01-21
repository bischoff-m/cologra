#pragma once

#include "../ColoringAlgorithm.hpp"
#include "../Heuristic.hpp"
#include "../definitions.hpp"

/**
 * Uses a heuristic to order the nodes and then colors them in that order.
 */
class OrderedSequential : public ColoringAlgorithm {
private:
  string heuristicId;

public:
  OrderedSequential(const nlohmann::json &params);
  OutType computeColoring(Graph graph) override;
};
