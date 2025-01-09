#include "InputLoader.hpp"
#include "../cologra/util/matrixToGraph.hpp"
#include "Paths.hpp"
#include "ProgressBarIterator.hpp"
#include <boost/graph/graphviz.hpp>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>

using json = nlohmann::json;
namespace fs = std::filesystem;

InputLoader::InputLoader() : index(), subsets(), graphs(), datasets() {}

InputLoader::~InputLoader() {
  if (index.has_value()) {
    for (auto it = index->begin(); it != index->end(); ++it) {
      delete it->second;
    }
  }
  if (subsets.has_value()) {
    for (auto it = subsets->begin(); it != subsets->end(); ++it) {
      it->second.clear();
    }
  }
  if (graphs.has_value()) {
    for (auto it = graphs->begin(); it != graphs->end(); ++it) {
      it->second.clear();
    }
  }
  if (datasets.has_value()) {
    for (auto it = datasets->begin(); it != datasets->end(); ++it) {
      it->second.clear();
    }
  }
}

void InputLoader::load(bool useCache) {
  // TODO: Load only matrices defined in targets
  loadSubsets();
  if (subsets->empty()) {
    cout << "No subsets found in " << Paths::subsets << endl;
    return;
  }
  loadIndex();
  indexToGraphs(useCache);
  loadDatasets();
}

map<string, Graph> InputLoader::getDataset(string id) {
  if (!datasets.has_value()) {
    throw invalid_argument("Datasets not loaded. Call loadDatasets() first.");
  }
  if (datasets->find(id) == datasets->end()) {
    throw invalid_argument(fmt::format("Dataset {} not found", id));
  }
  return (*datasets)[id];
}

void InputLoader::loadIndex() {
  map<string, SuiteSparseMatrix *> index;

  // Load index from file
  std::ifstream f(Paths::subsets / "index.json");
  json data = json::parse(f);
  for (const auto &matrix : data) {
    string id = matrix["id"];
    string name = matrix["name"];
    string group = matrix["group"];
    string filepath = matrix["filepath"];
    string format = matrix["format"];
    index[id] = new SuiteSparseMatrix(id, name, group, filepath, format);
  }
  this->index = index;
}

void InputLoader::loadSubsets() {
  map<string, vector<string>> subsets;

  // Iterate each subset file and get matrix IDs
  for (const auto &entry : fs::directory_iterator(Paths::subsets)) {
    if (!entry.is_regular_file())
      continue;
    // Match to ^subset\_(.*)\.json$
    std::regex re("^subset\\_(.*)\\.json$");
    std::smatch match;
    string filename = entry.path().filename().string();
    if (!std::regex_match(filename, match, re))
      continue;

    std::ifstream f(entry.path());
    json data = json::parse(f);

    vector<string> subset;
    for (const auto &matrix : data) {
      if (!matrix.is_string())
        throw invalid_argument("Matrix name must be a string");
      subset.push_back(matrix);
    }
    subsets[match[1]] = subset;
  }
  this->subsets = subsets;
}

void InputLoader::indexToGraphs(bool useCache) {
  if (!index.has_value()) {
    cout << "Index not loaded. Call loadIndex() first." << endl;
    return;
  }
  map<string, Graph> graphs;

  ProgressBarIterator<SuiteSparseMatrix *> progressBar(
      index->begin(), index->end());

  int count = 0;
  while (progressBar.begin() != progressBar.end()) {
    auto &[id, matrixPtr] = progressBar.next();

    // Get filename for cache
    string filename = id;
    replace(filename.begin(), filename.end(), '/', ' ');
    fs::path cacheFile = Paths::graphs / (filename + ".dot");

    if (useCache && fs::exists(cacheFile)) {
      // Load graph from cache
      Graph graph(0);
      dynamic_properties dp(ignore_other_properties);
      std::ifstream ifs(cacheFile);
      boost::read_graphviz(ifs, graph, dp);
      graphs[id] = graph;
      count++;
    } else {
      try {
        // Load matrix
        matrixPtr->loadMatrix();
        try {
          // Convert matrix to graph
          Graph graph = columnIntersectionGraph(matrixPtr->getMatrix());
          graphs[id] = graph;
          count++;

          if (useCache) {
            // Create directory if it doesn't exist
            fs::create_directories(Paths::graphs);
            // Save graph to cache
            std::ofstream ofs(cacheFile);
            boost::write_graphviz(ofs, graph);
          }
        } catch (const std::exception &e) {
          cout << "Error converting matrix " << matrixPtr->name
               << " to graph: " << e.what() << endl;
        }
        matrixPtr->unloadMatrix();
      } catch (const std::exception &e) {
        cout << "Error loading matrix " << id << ": " << e.what() << endl;
      }
    }
  }
  progressBar.finish();

  int total = index->size();
  cout << fmt::format("Loaded {} out of {} matrices", count, total) << endl;
  this->graphs = graphs;
}

void InputLoader::loadDatasets() {
  if (!subsets.has_value() || !graphs.has_value()) {
    cout << "Subsets or graphs not loaded. Call loadSubsets() and "
            "indexToGraphs() first."
         << endl;
    return;
  }

  map<string, map<string, Graph>> datasets;
  for (const auto &[subsetName, subset] : *subsets) {
    map<string, Graph> dataset;
    for (const string &matrixId : subset) {
      if (graphs->find(matrixId) == graphs->end()) {
        cout << "Matrix " << matrixId << " not found in graphs" << endl;
        continue;
      }
      dataset[matrixId] = (*graphs)[matrixId];
    }
    datasets[subsetName] = dataset;
  }
  this->datasets = datasets;
}
