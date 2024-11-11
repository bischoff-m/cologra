#include "definitions.hpp"
#include <Eigen/SparseCore>
#include <boost/graph/adjacency_list.hpp>

Graph columnIntersectionGraph(const Eigen::SparseMatrix<double> &matrix) {
  int numCols = matrix.cols();
  Graph graph(numCols);

  // Iterate through each row
  for (int row = 0; row < matrix.rows(); row++) {
    std::unordered_set<int> columnsNonZero;

    // Find columns with non-zero entries in the row
    for (int col = 0; col < numCols; col++)
      if (matrix.coeff(row, col) != 0)
        columnsNonZero.insert(col);

    // Add edges between columns with non-zero entries in the same row
    for (auto it1 = columnsNonZero.begin(); it1 != columnsNonZero.end(); it1++)
      for (auto it2 = std::next(it1); it2 != columnsNonZero.end(); it2++)
        add_edge(*it1, *it2, graph);
  }

  return graph;
}
