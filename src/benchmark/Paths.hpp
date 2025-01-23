#pragma once

#include <filesystem>
namespace fs = std::filesystem;

struct Paths {
  static const fs::path shared;
  static const fs::path subsets;
  static const fs::path matrices;
  static const fs::path graphs;
  static const fs::path benchmarks;
};
