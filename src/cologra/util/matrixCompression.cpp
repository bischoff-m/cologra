#include "matrixCompression.hpp"


Eigen::SparseMatrix<double> compressMatrixColumnwise(Eigen::SparseMatrix<double> &mat, Coloring &coloring){
    Eigen::SparseMatrix<double> compressed_matrix(mat.rows(), coloring.first);
    std::vector<Eigen::Triplet<double>> tripletList;
    tripletList.reserve(mat.nonZeros());
    for (int k=0; k<mat.outerSize(); ++k){
        for (Eigen::SparseMatrix<double>::InnerIterator it(mat,k); it; ++it)
        {
            tripletList.push_back(Eigen::Triplet<double>(it.row(),(*coloring.second)[it.col()],it.value()));
        }
    }
    compressed_matrix.setFromTriplets(tripletList.begin(), tripletList.end());
    return compressed_matrix;
}


Eigen::SparseMatrix<double> compressMatrixRowwise(Eigen::SparseMatrix<double> &mat, Coloring &coloring){
    Eigen::SparseMatrix<double> compressed_matrix(coloring.first, mat.cols());
    std::vector<Eigen::Triplet<double>> tripletList;
    tripletList.reserve(mat.nonZeros());
    for (int k=0; k<mat.outerSize(); ++k){
        for (Eigen::SparseMatrix<double>::InnerIterator it(mat,k); it; ++it)
        {
            tripletList.push_back(Eigen::Triplet<double>((*coloring.second)[it.row()], it.col(),it.value()));
        }
    }
    compressed_matrix.setFromTriplets(tripletList.begin(), tripletList.end());
    return compressed_matrix;
}