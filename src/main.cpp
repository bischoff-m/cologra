#include "coloring.cpp"
#include <Eigen/SparseCore>

int main(int argc, char **argv) {
  Eigen::SparseMatrix<double> mat =
      readMatrixMarket("data/matrix/can___24.mtx");
  printf("Read a sparse matrix with %ld rows and %ld columns\n",
      mat.rows(),
      mat.cols());
  return 0;
}
