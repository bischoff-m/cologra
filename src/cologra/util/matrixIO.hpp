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

/**
 * Write a sparse matrix into a matrix market file.
 *
 * @param outfile Path to the matrix market file
 * @param mat Sparse matrix
 */
void writeMatrixMarket(std::filesystem::path outfile, Eigen::SparseMatrix<double> mat);
