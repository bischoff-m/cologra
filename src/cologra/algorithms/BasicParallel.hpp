#pragma once

#include "../ColoringAlgorithm.hpp"
#include "../definitions.hpp"

/**
 * TODO: Document
 */
class BasicParallel : public ColoringAlgorithm {
public:
  BasicParallel();
  OutType computeColoring(Graph graph) override;
  OutType computeDist2Coloring(Graph graph);
};
