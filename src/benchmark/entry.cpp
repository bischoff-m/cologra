#include "../cologra/algorithms/BasicParallel.hpp"
#include "../cologra/algorithms/BasicSequential.hpp"
#include "../cologra/algorithms/BoostSequential.hpp"
#include "../cologra/algorithms/OrderedSequential.hpp"
#include "../cologra/algorithms/RandomPermutationQueue/RandomPermutationQueue.hpp"
#include "Benchmark.hpp"
#include "BenchmarkResult.hpp"
#include <fmt/core.h>

ColoringAlgorithm *createAlgorithm(string id, json params) {
  if (id == "BasicSequential") {
    return new BasicSequential();
  } else if (id == "BoostSequential") {
    return new BoostSequential();
  } else if (id == "OrderedSequential") {
    return new OrderedSequential(params);
  } else if (id == "BasicParallel") {
    return new BasicParallel();
  } else if (id == "RandomPermutationQueue") {
    return new RandomPermutationQueue(params);
  } else {
    throw invalid_argument(fmt::format("Algorithm {} not found", id));
  }
}

void runBenchmark() {
  // clang-format off
  vector<string> datasetIds = {
    "small_mtx"
    // "small_mtx_3elements"
  };
  vector<string> algorithmIds = {
    "BasicSequential",
    "BoostSequential",
    "OrderedSequential",
    "BasicParallel",
    "RandomPermutationQueue"
  };
  json params = {
    {"heuristic", "maxDegreeFirst"},
    {"numPermutations", 20}
  };
  // clang-format on

  vector<BenchmarkTarget> targets = {
      {datasetIds, algorithmIds, params},
  };

  Benchmark benchmark(targets);
  benchmark.measure(createAlgorithm);
}
