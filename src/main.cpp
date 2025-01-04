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

int main(int argc, char **argv) {
  std::filesystem::path path("data/matrix/can___24.mtx");
  Eigen::SparseMatrix<double> matrix = readMatrixMarket(path);
  Graph graph = columnIntersectionGraph(matrix);

  vector<VerticesSizeType> colorVec(boost::num_vertices(graph));
  ColorMap coloring(&colorVec.front(), get(vertex_index, graph));

  ColoringAlgorithm *algorithm = new BasicSequential();
  VerticesSizeType numColors = algorithm->computeColoring(graph, coloring);
  printf("Number of colors used: %ld\n", numColors);

  filesystem::path file("data/graph.dot");
  graphToDot(file, graph, coloring);

  return 0;
}
