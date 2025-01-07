#pragma once

#include "../ColoringAlgorithm.hpp"
#include "../definitions.hpp"

/**
 * TODO: Document
 */
class BasicParallel : public ColoringAlgorithm {
public:
  BasicParallel();
  VerticesSizeType computeColoring(Graph graph, ColorMap coloring) override;
  VerticesSizeType computeDist2Coloring(Graph graph, ColorMap coloring) override;
};
