#pragma once

#include <cstdint>
#include <random>

namespace graph_approx {

class Graph;

// Karger's randomized min-cut algorithm.
// Runs `trials` independent edge-contraction rounds and returns the smallest
// cut found. With trials=1 reproduces the original AAva behavior.
uint64_t karger(const Graph& g, int trials, std::mt19937& rng);

}  // namespace graph_approx
