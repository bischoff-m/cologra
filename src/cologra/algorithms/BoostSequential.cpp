#include "BoostSequential.hpp"
#include "../util/coloring.hpp"
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <nlohmann/json.hpp>

BoostSequential::BoostSequential()
    : ColoringAlgorithm(
          nlohmann::json(), AlgorithmId("BoostSequential", "1.0")) {}

Coloring BoostSequential::computeColoring(Graph graph) {
  auto [colorVec, colorIter] = getEmptyColoring(graph);
  Color numColors = sequential_vertex_coloring(graph, *colorIter);
  return {numColors, std::move(colorVec)};
}
