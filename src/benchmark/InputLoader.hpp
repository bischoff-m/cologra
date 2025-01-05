#pragma once

#include "../cologra/definitions.hpp"
#include "SuiteSparseMatrix.hpp"
#include <map>
#include <optional>
#include <string>
#include <vector>

using namespace std;

class InputLoader {
private:
  std::optional<map<string, SuiteSparseMatrix *>> index;
  std::optional<map<string, vector<string>>> subsets;
  std::optional<map<string, Graph>> graphs;
  std::optional<map<string, map<string, Graph>>> datasets;

  void loadIndex();
  void loadSubsets();
  void indexToGraphs(bool useCache = false);
  void loadDatasets();

public:
  InputLoader();
  ~InputLoader();
  void load(bool useCache = false);
  map<string, Graph> getDataset(string id);
};
