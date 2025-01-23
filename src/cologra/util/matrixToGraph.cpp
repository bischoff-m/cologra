#include "matrixToGraph.hpp"
#include <boost/graph/adjacency_list.hpp>

std::map<GraphRepresentation, std::string> reprToString = {
    {GraphRepresentation::COLUMN_INTERSECTION, "COLUMN_INTERSECTION"},
    {GraphRepresentation::ROW_INTERSECTION, "ROW_INTERSECTION"},
    {GraphRepresentation::ADJACENCY, "ADJACENCY"}};

std::string representationToString(GraphRepresentation repr) {
  return reprToString.at(repr);
}

GraphRepresentation representationFromString(const std::string &repr) {
  for (const auto &pair : reprToString) {
    if (pair.second == repr) {
      return pair.first;
    }
  }
  throw std::runtime_error("Unknown graph representation: " + repr);
}

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
        // Only add an edge if it does not already exist
        if (!boost::edge(*it1, *it2, graph).second)
          boost::add_edge(*it1, *it2, graph);
  }

  return graph;
}

Graph rowIntersectionGraph(const Eigen::SparseMatrix<double> &matrix) {
  int numRows = matrix.rows();
  Graph graph(numRows);

  // Iterate through each row
  for (int col = 0; col < matrix.cols(); col++) {
    std::unordered_set<int> rowsNonZero;

    // Find columns with non-zero entries in the row
    for (int row = 0; row < numRows; row++)
      if (matrix.coeff(row, col) != 0)
        rowsNonZero.insert(row);

    // Add edges between columns with non-zero entries in the same row
    for (auto it1 = rowsNonZero.begin(); it1 != rowsNonZero.end(); it1++)
      for (auto it2 = std::next(it1); it2 != rowsNonZero.end(); it2++)
        // Only add an edge if it does not already exist
        if (!boost::edge(*it1, *it2, graph).second)
          boost::add_edge(*it1, *it2, graph);
  }

  return graph;
}

Graph adjacencyGraph(const Eigen::SparseMatrix<double> &matrix) {
  if (matrix.rows() != matrix.cols()) {
    printf("Matrix not square\n");
    return {};
  }
  int numCols = matrix.cols();
  Graph graph(numCols);

  for (int k = 0; k < matrix.outerSize(); ++k) {
    for (Eigen::SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it) {
      if (!boost::edge(it.row(), it.col(), graph).second) {
        boost::add_edge(it.row(), it.col(), graph);
      }
    }
  }

  return graph;
}
