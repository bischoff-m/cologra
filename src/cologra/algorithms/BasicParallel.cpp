#include "BasicParallel.hpp"
#include <Eigen/SparseCore>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/permutation_iterator.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include "../Heuristic.hpp"

using namespace std;

BasicParallel::BasicParallel(const nlohmann::json &params)
    : ColoringAlgorithm(params, AlgorithmId("BasicParallel", "1.0")) {
  if (!params.contains("heuristics"))
    throw invalid_argument("Missing heuristics parameter");
  if (!Heuristic::isHeuristic(params["heuristics"]))
    throw invalid_argument("Invalid heuristics parameter");
  heuristicIds = params["heuristics"];
}

ColorType sequenceDependentGreedyColoring(Graph graph, ColorMap coloring, VertexIteratorRange indices) {
  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(coloring, node, -1);

  // Iterate over all nodes in the given indices sequence
  ColorType numColors = 0;
  for (auto node : boost::make_iterator_range(boost::make_permutation_iterator(boost::vertices(graph).first, indices.first),boost::make_permutation_iterator(boost::vertices(graph).first, indices.second))) {
    // Get the neighbors of the node
    auto neighbors = boost::adjacent_vertices(node, graph);

    // Get the colors of the neighbors
    vector<ColorType> neighborColors;
    for (auto neighbor : boost::make_iterator_range(neighbors))
      neighborColors.push_back(coloring[neighbor]);

    // Find the smallest color not used by any of its neighbors
    ColorType color;
    for (color = 0; color < boost::num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    boost::put(coloring, node, color);
    numColors = max(numColors, color + 1);
  }
  return numColors;
}

ColorType BasicParallel::computeColoring(
    Graph graph, ColorMap coloring) {
  VertexIteratorRange identity_ordering = boost::vertices(graph);
  
  std::vector<VertexIteratorRange> index_iterators = {identity_ordering};
  ColorType numColors = std::numeric_limits<ColorType>::max();
  #pragma omp parallel for reduction( min : numColors )
  for (auto& indices : index_iterators){
    vector<ColorType> colorVec(boost::num_vertices(graph));
    ColorMap local_coloring(&colorVec.front(), get(vertex_index, graph));
    ColorType local_numColors = sequenceDependentGreedyColoring(graph, local_coloring, indices);
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