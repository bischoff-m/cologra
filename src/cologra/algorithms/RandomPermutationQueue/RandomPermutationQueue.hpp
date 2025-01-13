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
  OutType computeColoring(Graph graph) override;
  void assistIfParallel() override;
  void stopIfParallel() override;
};
