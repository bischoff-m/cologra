#include "../cologra/util/createAlgorithm.hpp"
#include "Benchmark.hpp"
#include "BenchmarkResult.hpp"
#include <fmt/core.h>

void runBenchmark() {
  vector<int> tryNumPermutations = {1,
      2,
      3,
      4,
      5,
      6,
      7,
      8,
      9,
      10,
      20,
      30,
      40,
      50,
      60,
      70,
      80,
      90,
      100,
      200,
      300,
      400,
      500,
      600,
      700,
      800,
      900,
      1000,
      2000};
  vector<BenchmarkTarget> targets = {};
  for (int numPermutations : tryNumPermutations) {
    fmt::print("numPermutations: {}\n", numPermutations);
    // clang-format off
    vector<string> datasetIds = {
      "small_mtx"
      // "small_mtx_3elements",
      // "medium_mtx"
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
