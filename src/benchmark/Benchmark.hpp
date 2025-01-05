#pragma once

#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "../cologra/ColoringAlgorithm.hpp"
#include "InputLoader.hpp"

using namespace std;
using json = nlohmann::json;

struct BenchmarkTarget {
  vector<string> datasetIds;
  vector<string> algorithmIds;
  json parameters;
};

typedef std::function<ColoringAlgorithm *(string id, json params)>
    AlgorithmFactory;

class Benchmark {
private:
  vector<BenchmarkTarget> targets;
  InputLoader loader;

public:
  Benchmark(vector<BenchmarkTarget> targets);
  ~Benchmark() = default;
  void measure(AlgorithmFactory getAlgorithm);
};
