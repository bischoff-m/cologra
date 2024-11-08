#include "../src/coloring.cpp"
#include <gtest/gtest.h>

TEST(GreedyColoring, IsValid) {
  Graph graph{
      {2, 3},
      {2},
      {0, 1, 3, 4},
      {0, 2},
      {2},
  };
  graph.sortNeighbors();
  Coloring coloring = greedyColoring(graph);
  EXPECT_TRUE(isDistance1Coloring(graph, coloring));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
