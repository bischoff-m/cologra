#pragma once

#include "../ColoringAlgorithm.hpp"
#include "../definitions.hpp"

using namespace cologra;

/**
 * TODO: Document
 */
class BasicParallel : public ColoringAlgorithm {
public:
  BasicParallel();
  Coloring computeColoring(Graph graph) override;
  Coloring computeDist2Coloring(Graph graph);
};
