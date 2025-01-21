#include "BoostSequential.hpp"
#include "../util/coloring.hpp"
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <nlohmann/json.hpp>

using namespace std;

BoostSequential::BoostSequential()
    : ColoringAlgorithm(
          nlohmann::json(), AlgorithmId("BoostSequential", "1.0")) {}

OutType BoostSequential::computeColoring(Graph graph) {
  ColorMap coloring = getEmptyColorMap(graph);
  ColorType numColors = sequential_vertex_coloring(graph, coloring);
  return {numColors, coloring};
}
