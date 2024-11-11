#include <Eigen/SparseCore>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <cologra/coloring.hpp>
#include <cologra/definitions.hpp>
#include <cologra/display.hpp>
#include <cologra/matrixIO.hpp>
#include <cologra/matrixToGraph.hpp>
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
  VerticesSizeType numColors = greedyColoring(graph, coloring);
  EXPECT_EQ(numColors, 3);
}

TEST_F(SampleGraph, IsDistance1Coloring) {
  EXPECT_FALSE(isDistance1Coloring(graph, coloring));
  greedyColoring(graph, coloring);
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

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
