#pragma once

#include "definitions.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using namespace std;
using json = nlohmann::json;

struct AlgorithmId {
  string name;
  string versionCode;

  AlgorithmId(string name, string versionCode);

  string toString() const;
};

class ColoringAlgorithm {
private:
  json parameters;
  AlgorithmId algorithmId;
  std::optional<AlgorithmId> forkFrom;

public:
  ColoringAlgorithm(const json &params, AlgorithmId algorithmId);
  ColoringAlgorithm(
      const json &params, AlgorithmId algorithmId, AlgorithmId forkFrom);
  ~ColoringAlgorithm() = default;

  string getId() const;
  json getParameters() const;

  /**
   * Compute a coloring for the given graph.
   *
   * The coloring is stored in the coloring map. The keys are the vertices of
   * the graph and the values are the colors. The colors are integers starting
   * from 0. The number of colors used is returned. If using MPI, this method
   * is only called on the master node.
   *
   * @param graph Graph to color
   * @param coloring This map will be filled with the coloring
   * @return Number of colors used
   */
  virtual VerticesSizeType computeColoring(Graph graph, ColorMap coloring) = 0;

  /**
   * This method will be called before every call to computeColoring to allow
   * for parallel algorithms. It is called for every node, also the master node.
   */
  virtual void assistIfParallel();

  virtual void stopIfParallel();
};
