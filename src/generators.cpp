#include "graph_approx/generators.h"
#include "graph_approx/graph.h"

#include <algorithm>
#include <random>
#include <utility>
#include <vector>

namespace graph_approx {
using namespace std;

namespace {

// Build a CSR Graph from a sorted list of directed edges. Each entry of `edges`
// is (u, v) and contributes one entry to the CSR neighbours array.
Graph csr_from_sorted_edges(int n, const vector<pair<int, int>>& edges) {
	vector<int> offsets(n + 1, 0);
	vector<int> neighbours(edges.size());

	int previous = 0;
	int shift = 0;
	for (size_t i = 0; i < edges.size(); ++i) {
		int u = edges[i].first;
		int v = edges[i].second;
		if (u != previous) {
			for (int j = previous + 1; j <= u; ++j) offsets[j] = shift;
			previous = u;
		}
		neighbours[i] = v;
		++shift;
	}
	for (int j = previous + 1; j <= n; ++j) offsets[j] = shift;

	return Graph(std::move(offsets), std::move(neighbours));
}

}  // namespace

Graph scale_free(int n, std::mt19937& rng) {
	// Logic preserved from AAva/algorithms/ScaleFreeNetworks.cpp::scaleFree.
	vector<pair<int, int>> edges;
	edges.push_back({0, 1});
	edges.push_back({1, 0});
	edges.push_back({1, 2});
	edges.push_back({2, 1});
	edges.push_back({2, 0});
	edges.push_back({0, 2});

	for (int i = 3; i < n; ++i) {
		uniform_int_distribution<int> pick(0, static_cast<int>(edges.size()) - 1);
		auto random_edge = edges[pick(rng)];
		edges.push_back({i, random_edge.first});
		edges.push_back({random_edge.first, i});
		edges.push_back({random_edge.second, i});
		edges.push_back({i, random_edge.second});
	}

	sort(edges.begin(), edges.end());
	return csr_from_sorted_edges(n, edges);
}

}  // namespace graph_approx
