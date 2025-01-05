#include "SuiteSparseMatrix.hpp"
#include "../cologra/util/matrixIO.hpp"
#include "Paths.hpp"
#include <Eigen/SparseCore>

SuiteSparseMatrix::SuiteSparseMatrix(
    string id, string name, string group, string filepath, string format)
    : id(id), name(name), group(group), filepath(filepath), format(format) {}

SuiteSparseMatrix::~SuiteSparseMatrix() { unloadMatrix(); }

Eigen::SparseMatrix<double> SuiteSparseMatrix::getMatrix() const {
  if (!matrix.has_value())
    throw logic_error("Matrix is not loaded. Because matrices can be large, "
                      "they are not loaded until requested. Please call "
                      "loadMatrix() before trying to access the matrix.");
  return matrix.value();
}

void SuiteSparseMatrix::loadMatrix() {
  if (this->matrix.has_value())
    return;
  if (this->format == "MM") {
    this->matrix = readMatrixMarket(Paths::matrices / this->filepath);
  } else if (format == "RB") {
    throw logic_error("Not yet implemented");
  } else if (format == "mat") {
    throw logic_error("Not yet implemented");
  } else {
    throw invalid_argument("Invalid format");
  }
}

void SuiteSparseMatrix::unloadMatrix() { this->matrix.reset(); }
