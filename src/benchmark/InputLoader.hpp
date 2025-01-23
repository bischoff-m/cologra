#pragma once

#include "../cologra/definitions.hpp"
#include "../cologra/util/matrixToGraph.hpp"
#include "SuiteSparseMatrix.hpp"
#include <map>
#include <optional>
#include <string>
#include <vector>

using namespace std;
using namespace cologra;

class InputLoader {
private:
  std::optional<map<string, SuiteSparseMatrix *>> index;
  std::optional<map<string, vector<string>>> subsets;
  std::optional<map<string, Graph>> graphs;
  std::optional<map<string, map<string, Graph>>> datasets;

  void loadIndex(vector<string> subsetIds);
  void loadSubsets();
  void indexToGraphs(
      GraphRepresentation graphRepresentation, bool useCache = false);
  void loadDatasets(vector<string> subsetIds);

public:
  InputLoader();
  ~InputLoader();
  void load(vector<string> subsetIds,
      GraphRepresentation graphRepresentation,
      bool useGraphCache = false);
  map<string, Graph> getDataset(string id);
};
