#pragma once

#include <random>

namespace graph_approx {

class Graph;

// Exact global clustering coefficient. O(N^3) — for verification only.
long double clustering(const Graph& g);

// Bhatia's uniform-wedge sampling estimator.
long double uniform_wedge(const Graph& g, int sample_sz, std::mt19937& rng);

// Naive approximation: sample s nodes uniformly and check whether two random
// neighbours form a triangle.
double approx_clustering_naive(const Graph& g, int s, std::mt19937& rng);

}  // namespace graph_approx
