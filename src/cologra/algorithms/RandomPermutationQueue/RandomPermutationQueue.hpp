#pragma once

#include "../../ColoringAlgorithm.hpp"
#include "../../definitions.hpp"

/**
 * TODO: Add description
 */
class RandomPermutationQueue : public ColoringAlgorithm {
private:
  std::string heuristicId;
  int numPermutations;

public:
  RandomPermutationQueue(const nlohmann::json &params);
  VerticesSizeType computeColoring(Graph graph, ColorMap coloring) override;
  void assistIfParallel() override;
  void stopIfParallel() override;
};
