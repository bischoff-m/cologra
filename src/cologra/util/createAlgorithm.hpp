#pragma once

#include "../ColoringAlgorithm.hpp"

std::unique_ptr<ColoringAlgorithm> createAlgorithm(std::string id, json params);
