#pragma once

#include "../definitions.hpp"
#include <Eigen/SparseCore>

/**
 * Compute the undirected column intersection graph of the given matrix.
 *
 * @param matrix Input matrix
 * @return Column intersection graph
 */
Graph columnIntersectionGraph(const Eigen::SparseMatrix<double> &matrix);

/**
 * Compute the undirect graph interpreting the matrix as adjacency matrix.
 *
 * @param matrix Input matrix
 * @return adjacency graph
 */
Graph adjacencyGraph(const Eigen::SparseMatrix<double> &matrix);