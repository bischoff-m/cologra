#pragma once

#include <Eigen/SparseCore>
#include <filesystem>

/**
 * Read a matrix market file and return the sparse matrix.
 *
 * @param infile Path to the matrix market file
 * @return Sparse matrix
 */
Eigen::SparseMatrix<double> readMatrixMarket(std::filesystem::path infile);
