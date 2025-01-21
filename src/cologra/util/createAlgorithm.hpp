#pragma once

#include "../ColoringAlgorithm.hpp"
#include <fmt/core.h>
#include "../algorithms/BasicSequential.hpp"
#include "../algorithms/BasicParallel.hpp"
#include "../algorithms/OrderedSequential.hpp"
#include "../algorithms/BoostSequential.hpp"
#include "../algorithms/RandomPermutationQueue/RandomPermutationQueue.hpp"


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