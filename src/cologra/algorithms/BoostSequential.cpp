#include "BoostSequential.hpp"
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <nlohmann/json.hpp>

using namespace std;

BoostSequential::BoostSequential()
    : ColoringAlgorithm(
          nlohmann::json(), AlgorithmId("BoostSequential", "1.0")) {}

ColorType BoostSequential::computeColoring(
    Graph graph, ColorMap coloring) {
  return sequential_vertex_coloring(graph, coloring);
}
