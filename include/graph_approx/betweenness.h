#pragma once

#include <random>
#include <vector>

namespace graph_approx {

class Graph;

// Exact betweenness centrality (Brandes, 2001). O(VE).
std::vector<double> brandes(const Graph& g);

// 2-approximation of the graph's vertex diameter via single BFS from a random source.
int vertex_diameter_2approx(const Graph& g, std::mt19937& rng);

// Riondato-Kornaropoulos sampling estimator for betweenness centrality.
// Caller controls the (c, eps, delta) trade-off — these are no longer overwritten.
std::vector<double> riondato(const Graph& g, double c, double eps, double delta,
                             std::mt19937& rng);

}  // namespace graph_approx
