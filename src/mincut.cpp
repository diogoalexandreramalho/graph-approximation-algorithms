#include "graph_approx/mincut.h"
#include "graph_approx/graph.h"

#include <algorithm>
#include <random>
#include <utility>
#include <vector>

namespace graph_approx {
using namespace std;

namespace {

// Path-compressed, union-by-rank disjoint-set forest. Ported from
// AAva/structures/Graph.h's DisjointSet (kept private to this TU).
struct DisjointSet {
	vector<int> id;
	vector<int> rank_;

	explicit DisjointSet(int N) : id(N), rank_(N, 1) {
		for (int i = 0; i < N; ++i) id[i] = i;
	}

	int find(int i) {
		if (i != id[i]) id[i] = find(id[i]);
		return id[i];
	}

	void unite(int i, int j) {
		i = find(i);
		j = find(j);
		if (i == j) return;
		if (rank_[i] > rank_[j]) id[j] = i;
		else if (rank_[i] < rank_[j]) id[i] = j;
		else { id[j] = i; ++rank_[i]; }
	}

	bool same_set(int i, int j) { return find(i) == find(j); }
};

// Single Karger contraction round. Faithfully reproduces AAva's CalcMinCut:
//   - biased shuffle (index 0 is never moved to a later position),
//   - contract until `target_components` super-nodes remain,
//   - count edges still crossing between components.
uint64_t calc_min_cut(vector<pair<int, int>> edges, int N, int target_components,
                      std::mt19937& rng) {
	const int E = static_cast<int>(edges.size());

	// AAva: `for (i=E-1; i>0; i--) swap(edges[i], edges[rand() % i + 1]);`
	// `rand() % i + 1` parses as `(rand() % i) + 1`, range [1, i].
	for (int i = E - 1; i > 0; --i) {
		uniform_int_distribution<int> pick(1, i);
		swap(edges[i], edges[pick(rng)]);
	}

	DisjointSet ds(N);
	int components = N;
	for (int i = 0; i < E && components > target_components; ++i) {
		if (ds.same_set(edges[i].first, edges[i].second)) continue;
		ds.unite(edges[i].first, edges[i].second);
		--components;
	}

	uint64_t cut = 0;
	for (const auto& [u, v] : edges) {
		if (!ds.same_set(u, v)) ++cut;
	}
	return cut;
}

}  // namespace

uint64_t karger(const Graph& g, int trials, std::mt19937& rng) {
	const int N = g.num_nodes();
	auto edges = g.edges();

	// AAva initial UB: mincut = N - 1.
	uint64_t mincut = static_cast<uint64_t>(N - 1);
	for (int i = 0; i < trials; ++i) {
		mincut = min(mincut, calc_min_cut(edges, N, 2, rng));
	}
	return mincut;
}

}  // namespace graph_approx
