#pragma once

#include "../cologra/definitions.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using namespace std;
using namespace cologra;
using json = nlohmann::json;

struct BenchmarkTarget {
  vector<string> datasetIds;
  vector<string> algorithmIds;
  json parameters;
};

struct MachineInfo {
  string osName;
  int64_t totalMemoryBytes;
  vector<string> cpuNames;
  vector<string> gpuNames;
};

struct AggregatedValues {
  int64_t totalTimeNs;
  int64_t averageTimeNs;
  int64_t sumNumColors;
};

struct ExecutionResult {
  int64_t duration;
  Vertex numColors;
  string matrixId;
  bool didFail;
  string logOut;
  string logErr;
};

struct BenchmarkResult {
  int64_t timestampMs;
  AggregatedValues aggregated;
  vector<ExecutionResult> results;
  string datasetId;
  string algorithmId;
  std::optional<string> parentAlgorithmId;
  json parameters;
  MachineInfo machineInfo;

  json toJson();
  static void writeMultiple(
      vector<BenchmarkResult> &results, const BenchmarkTarget &target);
};
