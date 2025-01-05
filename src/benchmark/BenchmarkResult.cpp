#include "BenchmarkResult.hpp"
#include "Paths.hpp"
#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <string>

void BenchmarkResult::writeToFile() {
  json output;

  ostringstream oss;
  time_t timestampSec = timestampMs / 1000;
  oss << put_time(gmtime(&timestampSec), "%Y-%m-%dT%H:%M:%SZ");
  output["timestamp"] = oss.str();

  output["results"] = json::array();
  for (const auto &result : results) {
    output["results"].push_back({{"duration", result.duration},
        {"numColors", result.numColors},
        {"matrixId", result.matrixId},
        {"didFail", result.didFail},
        {"logOut", result.logOut},
        {"logErr", result.logErr}});
  }

  output["datasetId"] = datasetId;
  output["algorithmId"] = algorithmId;
  output["parameters"] = parameters;
  output["machineInfo"] = {{"osName", machineInfo.osName},
      {"totalMemoryBytes", machineInfo.totalMemoryBytes},
      {"cpuNames", machineInfo.cpuNames},
      {"gpuNames", machineInfo.gpuNames}};
  output["aggregated"] = {{"totalTime", aggregated.totalTime},
      {"averageTime", aggregated.averageTime},
      {"sumNumColors", aggregated.sumNumColors}};

  // Generate hash of input values
  string inputValues =
      fmt::format("{}{}{}", datasetId, algorithmId, parameters.dump());
  size_t hashValue = std::hash<string>{}(inputValues);
  stringstream hashStr;
  hashStr << hex << setw(6) << setfill('0') << (hashValue & 0xFFFFFF);

  // Format timestamp for filename
  ostringstream oss2;
  oss2 << put_time(gmtime(&timestampSec), "%Y-%m-%d_%H-%M-%S");
  string timestamp = oss2.str();

  // Create directory if it does not exist
  fs::create_directories(Paths::benchmarks);
  // Write to file
  string filename =
      fmt::format("benchmark_{}_{}.json", timestamp, hashStr.str());
  std::ofstream file(Paths::benchmarks / filename);
  file << output.dump(2);
  file.close();
}
