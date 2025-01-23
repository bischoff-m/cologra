#include "../cologra/util/createAlgorithm.hpp"
#include "Benchmark.hpp"
#include "Paths.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void runBenchmark() {
  ifstream file(Paths::shared / "targets.json");
  json targets = json::parse(file);

  Benchmark benchmark = Benchmark::fromJson(targets);
  benchmark.measure(createAlgorithm);
}
