#include "cologra/coloring.hpp"
#include "cologra/definitions.hpp"
#include "cologra/display.hpp"
#include "cologra/matrixToGraph.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <filesystem>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
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
  Graph graph(edgeArray, edgeArray + m, n);

  vector<VerticesSizeType> colorVec(boost::num_vertices(graph));
  ColorMap coloring(&colorVec.front(), get(vertex_index, graph));

  // VerticesSizeType numColors = sequential_vertex_coloring(graph, coloring);

  VerticesSizeType numColors = greedyColoring(graph, coloring);
  printf("Number of colors used: %ld\n", numColors);

  filesystem::path file("data/graph.dot");
  graphToDot(file, graph, coloring);

  return 0;
}
