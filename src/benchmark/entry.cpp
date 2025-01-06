#include "../cologra/algorithms/BasicSequential.hpp"
#include "../cologra/algorithms/BoostSequential.hpp"
#include "../cologra/algorithms/OrderedSequential.hpp"
#include "Benchmark.hpp"
#include <fmt/core.h>

ColoringAlgorithm *createAlgorithm(string id, json params) {
  if (id == "BasicSequential") {
    return new BasicSequential();
  } else if (id == "BoostSequential") {
    return new BoostSequential();
  } else if (id == "OrderedSequential") {
    return new OrderedSequential(params);
  } else if (id == "BasicParallel") {
    return new BasicParallel(params);
  } else {
    throw invalid_argument(fmt::format("Algorithm {} not found", id));
  }
}

void runBenchmark() {
  vector<BenchmarkTarget> targets = {{{"small_mtx"},
      {"BasicSequential", "BoostSequential", "OrderedSequential", "BasicParallel"},
      {{"heuristic", "maxDegreeFirst"}}}};

  Benchmark benchmark(targets);
  benchmark.measure(createAlgorithm);
}
