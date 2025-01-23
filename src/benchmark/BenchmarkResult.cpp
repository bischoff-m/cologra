#include "BenchmarkResult.hpp"
#include "Paths.hpp"
#include <fmt/core.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

json BenchmarkResult::toJson() {
  json output;

  ostringstream oss;
  time_t timestampSec = timestampMs / 1000;
  oss << put_time(gmtime(&timestampSec), "%Y-%m-%dT%H:%M:%SZ");
  output["timestamp"] = oss.str();

  output["results"] = json::array();
  for (const auto &result : results) {
    output["results"].push_back({{"duration", result.duration},
        {"numColors", result.numColors},
        {"coloring", result.coloring},
        {"matrixId", result.matrixId},
        {"graphSize", result.graphSize},
        {"didFail", result.didFail}});
  }

  output["datasetId"] = datasetId;
  output["algorithmId"] = algorithmId;
  output["parameters"] = parameters;

  output["machineInfo"] = {{"osName", machineInfo.osName},
      {"totalMemoryBytes", machineInfo.totalMemoryBytes},
      {"cpuNames", machineInfo.cpuNames},
      {"gpuNames", machineInfo.gpuNames},
      {"mpiSize", machineInfo.mpiSize}};

  output["aggregated"] = {{"totalTimeNs", aggregated.totalTimeNs},
      {"averageTimeNs", aggregated.averageTimeNs},
      {"sumNumColors", aggregated.sumNumColors}};
  output["graphRepresentation"] = representationToString(graphRepresentation);

  return output;
}

void BenchmarkResult::writeMultiple(
    vector<BenchmarkResult> &results, const BenchmarkTarget &target) {
  json output;
  output["results"] = json::array();
  for (auto &result : results) {
    output["results"].push_back(result.toJson());
  }
  output["target"] = {{"datasetIds", target.datasetIds},
      {"algorithmIds", target.algorithmIds},
      {"graphRepresentation",
          representationToString(target.graphRepresentation)},
      {"parameters", target.parameters}};

  // Generate hash of input values
  string uniqueValue;
  vector<string> datasetIds = target.datasetIds;
  vector<string> algorithmIds = target.algorithmIds;
  sort(datasetIds.begin(), datasetIds.end());
  sort(algorithmIds.begin(), algorithmIds.end());
  for (const auto &datasetId : target.datasetIds) {
    uniqueValue += datasetId;
  }
  for (const auto &algorithmId : target.algorithmIds) {
    uniqueValue += algorithmId;
  }
  uniqueValue += representationToString(target.graphRepresentation);
  uniqueValue += target.parameters.dump();
  size_t hashValue = std::hash<string>{}(uniqueValue);
  stringstream hashStr;
  hashStr << hex << setw(6) << setfill('0') << (hashValue & 0xFFFFFF);

  // Format timestamp for filename
  time_t timestampSec = results[0].timestampMs / 1000;
  ostringstream oss;
  oss << put_time(gmtime(&timestampSec), "%Y-%m-%d_%H-%M-%S");
  string timestamp = oss.str();

  // Create directory if it does not exist
  fs::create_directories(Paths::benchmarks);
  // Write to file
  string filename =
      fmt::format("benchmark_{}_{}.json", timestamp, hashStr.str());
  std::ofstream file(Paths::benchmarks / filename);
  file << output.dump(2);
  file.close();
}
