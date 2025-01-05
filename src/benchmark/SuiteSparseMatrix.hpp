#pragma once

#include "../cologra/definitions.hpp"
#include <Eigen/SparseCore>
#include <filesystem>

using namespace std;

class SuiteSparseMatrix {
private:
  std::optional<Eigen::SparseMatrix<double>> matrix;
  string filepath;
  string format;

public:
  const string id;
  const string name;
  const string group;

  SuiteSparseMatrix(
      string id, string name, string group, string filepath, string format);
  ~SuiteSparseMatrix();

  Eigen::SparseMatrix<double> getMatrix() const;

  void loadMatrix();
  void unloadMatrix();
};
