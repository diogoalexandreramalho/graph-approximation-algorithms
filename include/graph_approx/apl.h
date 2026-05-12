#pragma once

#include <random>

namespace graph_approx {

class Graph;

// HyperANF estimate of the average path length.
// b: log2 of the number of HyperLogLog registers per node (m = 2^b).
// Returns the average distance, computed from the converged neighbourhood function.
long double apl(const Graph& g, int b, std::mt19937& rng);

}  // namespace graph_approx
