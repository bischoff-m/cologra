#include "createAlgorithm.hpp"
#include "../algorithms/BasicParallel.hpp"
#include "../algorithms/BasicSequential.hpp"
#include "../algorithms/BoostSequential.hpp"
#include "../algorithms/OrderedSequential.hpp"
#include "../algorithms/RandomPermutationQueue/RandomPermutationQueue.hpp"
#include <fmt/core.h>

using namespace std;

unique_ptr<ColoringAlgorithm> createAlgorithm(string id, json params) {
  if (id == "BasicSequential") {
    return make_unique<BasicSequential>();
  } else if (id == "BoostSequential") {
    return make_unique<BoostSequential>();
  } else if (id == "OrderedSequential") {
    return make_unique<OrderedSequential>(params);
  } else if (id == "BasicParallel") {
    return make_unique<BasicParallel>();
  } else if (id == "RandomPermutationQueue") {
    return make_unique<RandomPermutationQueue>(params);
  } else {
    throw invalid_argument(fmt::format("Algorithm {} not found", id));
  }
}
