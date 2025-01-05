#include "../cologra/algorithms/BasicSequential.hpp"
#include "../cologra/algorithms/BoostSequential.hpp"
#include "Benchmark.hpp"
#include <fmt/core.h>

ColoringAlgorithm *createAlgorithm(string id, json params) {
  if (id == "BasicSequential") {
    return new BasicSequential();
  } else if (id == "BoostSequential") {
    return new BoostSequential();
  } else {
    throw invalid_argument(fmt::format("Algorithm {} not found", id));
  }
}

void runBenchmark() {
  vector<BenchmarkTarget> targets = {
      {{"small_mtx"}, {"BasicSequential", "BoostSequential"}, {}}};
  Benchmark benchmark(targets);
  benchmark.measure(createAlgorithm);
}
