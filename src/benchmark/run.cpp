#include "../cologra/algorithms/BasicSequential.hpp"
#include "../cologra/algorithms/BoostSequential.hpp"
#include "../cologra/definitions.hpp"
#include "../cologra/util/matrixIO.hpp"
#include "../cologra/util/matrixToGraph.hpp"
#include <Eigen/SparseCore>
#include <chrono>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>

using namespace std;
using namespace std::chrono;
using namespace indicators;
using json = nlohmann::json;
namespace fs = std::filesystem;

const fs::path subsetDir = fs::current_path() / "suitesparse/data/subsets";
const fs::path matrixDir = fs::current_path() / "suitesparse/data/matrices";

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
      string id, string name, string group, string filepath, string format)
      : id(id), name(name), group(group), filepath(filepath), format(format) {}

  Eigen::SparseMatrix<double> getMatrix() const {
    if (!matrix.has_value())
      throw logic_error("Matrix is not loaded. Because matrices can be large, "
                        "they are not loaded until requested. Please call "
                        "loadMatrix() before trying to access the matrix.");
    return matrix.value();
  }

  void loadMatrix() {
    if (this->matrix.has_value())
      return;
    if (this->format == "MM") {
      this->matrix = readMatrixMarket(matrixDir / this->filepath);
    } else if (format == "RB") {
      throw logic_error("Not yet implemented");
    } else if (format == "mat") {
      throw logic_error("Not yet implemented");
    } else {
      throw invalid_argument("Invalid format");
    }
  }

  void unloadMatrix() { this->matrix.reset(); }
};

map<string, SuiteSparseMatrix *> loadIndex() {
  map<string, SuiteSparseMatrix *> index;
  std::ifstream f(subsetDir / "index.json");
  json data = json::parse(f);
  for (const auto &matrix : data) {
    string id = matrix["id"];
    string name = matrix["name"];
    string group = matrix["group"];
    string filepath = matrix["filepath"];
    string format = matrix["format"];
    index[id] = new SuiteSparseMatrix(id, name, group, filepath, format);
  }
  return index;
}

map<string, vector<string>> loadSubsets() {
  map<string, vector<string>> subsets;
  for (const auto &entry : fs::directory_iterator(subsetDir)) {
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
  return subsets;
}

/**
 * Convert all matrices to graphs. This unloads all matrices after converting
 * them to graphs.
 *
 * @param matrices Matrices to convert
 * @return Graphs
 */
map<string, Graph> convertToGraphs(map<string, SuiteSparseMatrix *> &matrices) {
  show_console_cursor(false);
  BlockProgressBar bar{
      option::MaxProgress{matrices.size()},
  };

  map<string, Graph> graphs;
  int count = 0;

  for (auto it = matrices.begin(); it != matrices.end(); ++it) {
    string id = it->first;
    SuiteSparseMatrix *matrixPtr = it->second;

    try {
      // Load matrix
      matrixPtr->loadMatrix();
      try {
        // Convert matrix to graph
        graphs[id] = columnIntersectionGraph(matrixPtr->getMatrix());
        matrixPtr->unloadMatrix();
        count++;
      } catch (const std::exception &e) {
        cout << "Error converting matrix " << matrixPtr->name
             << " to graph: " << e.what() << endl;
      }
    } catch (const std::exception &e) {
      cout << "Error loading matrix " << id << ": " << e.what() << endl;
    }
    // Show iteration as postfix text
    int i = std::distance(matrices.begin(), it);
    bar.set_option(option::PostfixText{
        fmt::format(" {}/{} {}", i + 1, matrices.size(), id)});
    bar.tick();
  }
  bar.mark_as_completed();
  show_console_cursor(true);

  int total = matrices.size();
  cout << fmt::format("Loaded {} out of {} matrices", count, total) << endl;
  return graphs;
}

void run() {
  cout << "Loading subsets" << endl;
  const map<string, vector<string>> subsets = loadSubsets();
  if (subsets.empty()) {
    cout << "No subsets found in " << subsetDir << endl;
    return;
  }

  cout << "Loading index" << endl;
  // Get graphs for all matrices that are contained in any subset
  map<string, SuiteSparseMatrix *> allMatrices = loadIndex();
  cout << fmt::format("Loaded {} matrices", allMatrices.size()) << endl;
  map<string, Graph> graphs = convertToGraphs(allMatrices);
  cout << fmt::format("Converted {} matrices to graphs", graphs.size()) << endl;

  for (const auto &[subsetName, subset] : subsets) {
    cout << "Running subset " << subsetName << endl;
    int64_t totalDuration = 0;
    for (const string &matrixId : subset) {
      if (graphs.find(matrixId) == graphs.end()) {
        cout << "Matrix " << matrixId << " not found in index" << endl;
        continue;
      }
      Graph &graph = graphs[matrixId];

      // Create empty coloring and initialize algorithm
      vector<VerticesSizeType> colorVec(boost::num_vertices(graph));
      ColorMap coloring(&colorVec.front(), get(vertex_index, graph));
      ColoringAlgorithm *algorithm = new BoostSequential();

      // Measure execution time
      auto t1 = high_resolution_clock::now();
      VerticesSizeType numColors = algorithm->computeColoring(graph, coloring);
      auto t2 = high_resolution_clock::now();

      auto duration = duration_cast<microseconds>(t2 - t1).count();
      totalDuration += duration;
    }
    cout << fmt::format("Total duration for subset {}: {} µs",
                subsetName,
                totalDuration)
         << endl;
  }
}
