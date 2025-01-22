#include "../cologra/util/createAlgorithm.hpp"
#include "Benchmark.hpp"
#include "BenchmarkResult.hpp"
#include <fmt/core.h>

void runBenchmark() {
  vector<int> tryNumPermutations = {10};
  vector<BenchmarkTarget> targets = {};
  for (int numPermutations : tryNumPermutations) {
    fmt::print("numPermutations: {}\n", numPermutations);
    // clang-format off
    vector<string> datasetIds = {
      // "small_mtx"
      "small_mtx_3elements",
      // "medium_mtx",
      // "only_netz4504",
    };
    vector<string> algorithmIds = {
      // "BasicSequential",
      // "BoostSequential",
      // "OrderedSequential",
      // "BasicParallel",
      "RandomPermutationQueue"
    };
    json params = {
      {"heuristic", "maxDegreeFirst"},
      {"numPermutations", numPermutations}
    };
    // clang-format on
    targets.push_back({datasetIds, algorithmIds, params});
  }

  Benchmark benchmark(targets);
  benchmark.measure(createAlgorithm);
}
