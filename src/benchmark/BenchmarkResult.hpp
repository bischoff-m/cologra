#pragma once

#include "../cologra/definitions.hpp"
#include "../cologra/util/matrixToGraph.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using namespace std;
using namespace cologra;
using json = nlohmann::json;

struct BenchmarkTarget {
  vector<string> datasetIds;
  vector<string> algorithmIds;
  GraphRepresentation graphRepresentation;
  json parameters;
};

struct MachineInfo {
  string osName;
  int64_t totalMemoryBytes;
  vector<string> cpuNames;
  vector<string> gpuNames;
  int mpiSize;
};

struct AggregatedValues {
  int64_t totalTimeNs;
  int64_t averageTimeNs;
  int64_t sumNumColors;
};

struct ExecutionResult {
  int64_t duration;
  Vertex numColors;
  ColorVector coloring;
  string matrixId;
  Vertex graphSize;
  bool didFail;
};

struct BenchmarkResult {
  int64_t timestampMs;
  AggregatedValues aggregated;
  vector<ExecutionResult> results;
  string datasetId;
  string algorithmId;
  std::optional<string> parentAlgorithmId;
  GraphRepresentation graphRepresentation;
  json parameters;
  MachineInfo machineInfo;

  json toJson();
  static void writeMultiple(
      vector<BenchmarkResult> &results, const BenchmarkTarget &target);
};
