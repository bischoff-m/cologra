#pragma once

#include "../../ColoringAlgorithm.hpp"
#include "../../definitions.hpp"

using namespace cologra;

/**
 * TODO: Add description
 */
class RandomPermutationQueue : public ColoringAlgorithm {
private:
  std::string heuristicId;
  int numPermutations;

public:
  RandomPermutationQueue(const nlohmann::json &params);
  Coloring computeColoring(Graph graph) override;
  void assistIfParallel() override;
  void stopIfParallel() override;
};
