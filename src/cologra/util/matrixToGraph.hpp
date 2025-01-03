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
