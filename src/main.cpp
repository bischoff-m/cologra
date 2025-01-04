#include "benchmark/run.cpp"
#include "cologra/algorithms/BasicSequential.hpp"
#include "cologra/definitions.hpp"
#include "cologra/util/display.hpp"
#include "cologra/util/matrixIO.hpp"
#include "cologra/util/matrixToGraph.hpp"
#include <Eigen/SparseCore>
#include <boost/graph/adjacency_list.hpp>
#include <filesystem>
#include <vector>

using namespace std;

void testFullChain() {
  // Read matrix
  filesystem::path path("data/matrix/can___24.mtx");
  Eigen::SparseMatrix<double> matrix = readMatrixMarket(path);

  // Compute column intersection graph
  Graph graph = columnIntersectionGraph(matrix);

  // Compute coloring
  vector<VerticesSizeType> colorVec(boost::num_vertices(graph));
  ColorMap coloring(&colorVec.front(), get(vertex_index, graph));
  ColoringAlgorithm *algorithm = new BasicSequential();
  VerticesSizeType numColors = algorithm->computeColoring(graph, coloring);
  printf("Number of colors used: %ld\n", numColors);

  // Save graph to DOT file
  filesystem::path file("data/graph.dot");
  graphToDot(file, graph, coloring);
}

int main(int argc, char **argv) {
  run();

  return 0;
}
