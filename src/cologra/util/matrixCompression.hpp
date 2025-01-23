#pragma once

#include "../definitions.hpp"
#include <Eigen/SparseCore>

using namespace cologra;

Eigen::SparseMatrix<double> compressMatrixColumnwise(Eigen::SparseMatrix<double> &matrix, Coloring &coloring);