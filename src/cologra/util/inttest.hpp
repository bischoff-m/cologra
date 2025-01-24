
#include <Eigen/SparseCore>
#include <Eigen/Dense>
#include "matrixCompression.hpp"

bool inttest(Eigen::SparseMatrix<double> &matrix, Coloring &coloring, int thoroughness=30){
    bool test_result = true;
    for (int i = 0; i<matrix.cols() && i < thoroughness; i++){
        
        Eigen::SparseMatrix<double> changed_matrix(matrix.rows(), matrix.cols());
        std::vector<Eigen::Triplet<double>> tripletList;
        tripletList.reserve(matrix.nonZeros());
        for (int k=0; k<matrix.outerSize(); ++k){
            for (Eigen::SparseMatrix<double>::InnerIterator it(matrix,k); it; ++it)
            {
                if (it.value()!=0){
                    tripletList.push_back(Eigen::Triplet<double>(it.row(), it.col(),(it.value()+i*it.col())));
                }
            }
        }
        changed_matrix.setFromTriplets(tripletList.begin(), tripletList.end());
        Eigen::SparseMatrix<double> compressed_matrix = compressMatrixColumnwise(changed_matrix, coloring);
        Eigen::VectorXd all_ones_large = Eigen::VectorXd::Ones(matrix.cols());
        Eigen::VectorXd all_ones_small = Eigen::VectorXd::Ones(compressed_matrix.cols());
        test_result = test_result && ((changed_matrix * all_ones_large) == (compressed_matrix * all_ones_small)); //&& compressed_matrix * all_ones_small !=Eigen::VectorXd::Zero(matrix.rows());
    
    }
    return test_result;
}