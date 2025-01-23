#include "matrixIO.hpp"
#include <fast_matrix_market/app/Eigen.hpp>
#include <fstream>
#include <string>

Eigen::SparseMatrix<double> readMatrixMarket(std::filesystem::path infile) {
  std::ifstream f(infile.string());
  Eigen::SparseMatrix<double> mat;
  fast_matrix_market::read_matrix_market_eigen(f, mat);
  f.close();
  return mat;
}


void writeMatrixMarket(std::filesystem::path outfile, Eigen::SparseMatrix<double> mat){
  std::ofstream f(outfile.string());
  fast_matrix_market::write_matrix_market_eigen(f, mat);
  f.close();
}