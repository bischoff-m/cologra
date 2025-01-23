#include "Paths.hpp"

const fs::path Paths::shared = fs::current_path() / "shared";
const fs::path Paths::subsets = fs::current_path() / "shared/subsets";
const fs::path Paths::matrices = fs::current_path() / "shared/matrices";
const fs::path Paths::graphs = fs::current_path() / "shared/graphs";
const fs::path Paths::benchmarks = fs::current_path() / "shared/benchmarks";
