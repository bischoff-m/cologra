#include <Eigen/SparseCore>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <cologra/algorithms/BasicSequential.hpp>
#include <cologra/definitions.hpp>
#include <cologra/util/coloring.hpp>
#include <cologra/util/display.hpp>
#include <cologra/util/matrixIO.hpp>
#include <cologra/util/matrixToGraph.hpp>
#include <filesystem>
#include <gtest/gtest.h>
#include <vector>

class SampleGraph : public ::testing::Test {
public:
  Graph graph;
  ColorMap coloring;

  SampleGraph() {
    enum nodes { A, B, C, D, E, n };
    Edge edgeArray[] = {Edge(A, C),
        Edge(B, B),
        Edge(B, D),
        Edge(B, E),
        Edge(C, B),
        Edge(C, D),
        Edge(D, E),
        Edge(E, A),
        Edge(E, B)};
    int m = sizeof(edgeArray) / sizeof(Edge);
    graph = Graph(edgeArray, edgeArray + m, n);
  }

  void SetUp() {
    colorVec = std::vector<VerticesSizeType>(boost::num_vertices(graph));
    coloring = ColorMap(&colorVec.front(), get(vertex_index, graph));
  }

private:
  std::vector<VerticesSizeType> colorVec;
};

TEST_F(SampleGraph, GreedyColoring) {
  ColoringAlgorithm *algorithm = new BasicSequential();
  VerticesSizeType numColors = algorithm->computeColoring(graph, coloring);
  EXPECT_EQ(numColors, 3);
}

TEST_F(SampleGraph, IsDistance1Coloring) {
  EXPECT_FALSE(isDistance1Coloring(graph, coloring));
  ColoringAlgorithm *algorithm = new BasicSequential();
  algorithm->computeColoring(graph, coloring);
  EXPECT_TRUE(isDistance1Coloring(graph, coloring));
}

TEST_F(SampleGraph, GraphToDot) {
  sequential_vertex_coloring(graph, coloring);

  std::filesystem::path file("graph.dot");
  graphToDot(file, graph, coloring);
  EXPECT_TRUE(std::filesystem::exists(file));

  std::filesystem::remove(file);
}

TEST(MatrixIO, ReadMatrixMarket) {
  std::filesystem::path infile("../../shared/matrices/AG-Monien/netz4504.mtx");
  Eigen::SparseMatrix<double> mat = readMatrixMarket(infile);
  EXPECT_EQ(mat.rows(), 1961);
  EXPECT_EQ(mat.cols(), 1961);
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

TEST(MatrixToGraph, AdjacencyGraph) {

  // 0 1 0 1
  // 1 0 0 0
  // 0 0 0 0
  // 1 0 0 0

  Eigen::SparseMatrix<double> mat(4, 4);
  mat.insert(0, 1) = 1;
  mat.insert(1, 0) = 1;
  mat.insert(3, 0) = 1;
  mat.insert(0, 3) = 1;

  Graph graph = adjacencyGraph(mat);
  EXPECT_EQ(boost::num_vertices(graph), 4);
  EXPECT_EQ(boost::num_edges(graph), 2);
}

TEST(MatrixToGraph, AdjacencyGraph2) {

  // 0 1 0 1
  // 1 0 1 0
  // 0 1 0 1
  // 1 0 1 0

  Eigen::SparseMatrix<double> mat(4, 4);
  mat.insert(0, 1) = 1;
  mat.insert(1, 0) = 1;
  mat.insert(3, 0) = 1;
  mat.insert(0, 3) = 1;
  mat.insert(2, 1) = 1;
  mat.insert(1, 2) = 1;
  mat.insert(3, 2) = 1;
  mat.insert(2, 3) = 1;

  Graph graph = adjacencyGraph(mat);
  EXPECT_EQ(boost::num_vertices(graph), 4);
  EXPECT_EQ(boost::num_edges(graph), 4);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
