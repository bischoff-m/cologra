#include "BasicParallel.hpp"
#include <Eigen/SparseCore>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include <omp.h>

using namespace std;

BasicParallel::BasicParallel()
    : ColoringAlgorithm(
          nlohmann::json(), AlgorithmId("BasicParallel", "1.0")) {}

VerticesSizeType BasicParallel::computeColoring(
    Graph graph, ColorMap coloring, std::vector<VertexMap> index_iterators) {
  ColorType numColors = std::numerical_limits<ColorType>::max();
  #pragma omp parallel for reduction( min : numColors )
  for (auto& indices : index_iterators){
    ColorMap local_coloring;
    ColorType local_numColors = sequenceDependentGreedyColoring(Graph graph, ColorMap local_coloring, VertexMap indices);
    #pragma omp critical
    {
      if (local_numColors<numColors){
        coloring = local_coloring;
        numColors = local_numColors;
      }
    }
  }
  return numColors;
}