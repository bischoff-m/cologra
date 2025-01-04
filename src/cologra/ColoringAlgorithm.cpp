#include "ColoringAlgorithm.hpp"
#include <fmt/core.h>
#include <regex>
#include <stdexcept>

AlgorithmId::AlgorithmId(string name, string versionCode) {
  // Check if name follows convention
  regex pattern("^([A-Z][a-z0-9]+)((\\d)|([A-Z0-9][a-z0-9]+))*([A-Z])?$");
  if (!regex_match(name, pattern)) {
    throw invalid_argument(
        fmt::format("Name must be in upper camel case. Got '{}'", name));
  }

  // Check if versionCode follows convention
  pattern = regex("^\\d+.\\d+(\\.\\d+)*$");
  if (!regex_match(versionCode, pattern)) {
    throw invalid_argument(fmt::format(
        "Version code must match the pattern ^\\d+.\\d+(\\.\\d+)*$. Got '{}'",
        versionCode));
  }

  this->name = name;
  this->versionCode = versionCode;
}

// Definition for ColoringAlgorithm constructors
ColoringAlgorithm::ColoringAlgorithm(
    const json &params, AlgorithmId algorithmId)
    : parameters(params), algorithmId(std::move(algorithmId)), forkFrom() {}

ColoringAlgorithm::ColoringAlgorithm(
    const json &params, AlgorithmId algorithmId, AlgorithmId forkFrom)
    : parameters(params), algorithmId(std::move(algorithmId)),
      forkFrom(std::move(forkFrom)) {}

json ColoringAlgorithm::getParameters() const { return parameters; }
