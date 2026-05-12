#pragma once

#include <random>

namespace graph_approx {

class Graph;

// Dorogovtsev-Mendes-Samukhin minimal scale-free model — preserves the logic
// of AAva/algorithms/ScaleFreeNetworks.cpp::scaleFree:
//   1. Start with a 3-node triangle (6 directed edges).
//   2. For each new node i ∈ [3, n): pick a uniformly random existing
//      directed edge and attach i to both of its endpoints (4 new directed
//      edges per added node).
// This is the generator that produced the 50 SFNs used in the report.
Graph scale_free(int n, std::mt19937& rng);

}  // namespace graph_approx
