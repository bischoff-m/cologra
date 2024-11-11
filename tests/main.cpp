#include <Eigen/SparseCore>
#include <boost/graph/adjacency_list.hpp>
#include <cologra/coloring.hpp>
#include <cologra/definitions.hpp>
#include <cologra/matrixIO.hpp>
#include <cologra/matrixToGraph.hpp>
#include <filesystem>
#include <gtest/gtest.h>

// TEST(GreedyColoring, IsValid) {
//   Graph graph{
//       {2, 3},
//       {2},
//       {0, 1, 3, 4},
//       {0, 2},
//       {2},
//   };
//   Coloring coloring = greedyColoring(graph);
//   EXPECT_TRUE(isDistance1Coloring(graph, coloring));
// }

TEST(MatrixIO, ReadMatrixMarket) {
  std::filesystem::path infile("../../data/matrix/can___24.mtx");
  Eigen::SparseMatrix<double> mat = readMatrixMarket(infile);
  EXPECT_EQ(mat.rows(), 24);
  EXPECT_EQ(mat.cols(), 24);
}

TEST(MatrixToGraph, ColumnIntersectionGraph) {
  Eigen::SparseMatrix<double> mat(4, 4);
  mat.insert(0, 0) = 1;
  mat.insert(0, 2) = 1;
  mat.insert(1, 1) = 1;
  mat.insert(1, 2) = 1;
  mat.insert(2, 0) = 1;
  mat.insert(3, 1) = 1;
  Graph graph = columnIntersectionGraph(mat);
  EXPECT_EQ(boost::num_vertices(graph), 4);
  EXPECT_EQ(boost::num_edges(graph), 2);
}

// TEST(Display, GraphToDot)

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
