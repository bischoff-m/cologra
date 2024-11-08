#include <Eigen/SparseCore>
#include <algorithm>
#include <fast_matrix_market/app/Eigen.hpp>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

typedef vector<int> Coloring;

class Graph : public vector<vector<int>> {
public:
  Graph(initializer_list<vector<int>> list) : vector<vector<int>>(list) {}

  /**
   * Sort neighbors of each node in ascending order. This is done to minimize
   * the look-up time for the smallest color not used by any of its neighbors.
   */
  void sortNeighbors() {
    for (vector<int> &neighbors : *this)
      sort(neighbors.begin(), neighbors.end());
  }
};

/**
 * Greedy sequential coloring
 *
 * Reference
 * ---------
 * Page 691 of the paper:
 * Gebremedhin et al. “What Color Is Your Jacobian? Graph Coloring
 * for Computing Derivatives.” SIAM Review 47, no. 4 (January 2005): 629–705.
 * https://doi.org/10.1137/S0036144504444711.
 *
 * @param graph Graph
 * @return Coloring
 */
Coloring greedyColoring(Graph graph) {
  int size = graph.size();
  Coloring coloring(size, -1);

  for (int node = 0; node < size; node++) {
    // Find smallest color not used by any of its neighbors
    int color;
    for (color = 0; color < size; color++) {
      bool available = true;
      for (int neighbor : graph[node])
        if (coloring[neighbor] == color) {
          available = false;
          break;
        }
      if (available)
        break;
    }

    // Assign the found color to the node
    coloring[node] = color;
  }
  return coloring;
}

/**
 * Check if the given coloring is a valid distance-1 coloring.
 * A coloring is distance-1 if no two adjacent nodes have the same color.
 *
 * @param graph Graph
 * @param coloring Coloring
 * @return True if the coloring is a valid distance-1 coloring
 */
bool isDistance1Coloring(Graph graph, Coloring coloring) {
  for (int node = 0; node < graph.size(); node++) {
    for (int neighbor : graph[node])
      if (coloring[node] == coloring[neighbor])
        return false;
  }
  return true;
}

/**
 * Read a matrix market file and return the sparse matrix.
 *
 * @param filename Path to the matrix market file
 * @return Sparse matrix
 */
Eigen::SparseMatrix<double> readMatrixMarket(string filename) {
  ifstream f(filename);
  Eigen::SparseMatrix<double> mat;
  fast_matrix_market::read_matrix_market_eigen(f, mat);
  return mat;
}
