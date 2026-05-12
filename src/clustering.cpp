#include "graph_approx/clustering.h"
#include "graph_approx/graph.h"

#include <cassert>
#include <random>
#include <vector>

namespace graph_approx {
using namespace std;

namespace {

// Find the smallest index whose accumulated wedge count is >= r.
int binary_search_wedge(int N, int r, const vector<int>& acc_wedge_count) {
	int L = 0;
	int R = N - 1;
	int res = -1;
	int itvl = -1;

	while (L <= R) {
		int guess = L + (R - L) / 2;
		itvl = guess;
		if (acc_wedge_count[guess] == r) {
			res = guess;
			R = guess - 1;
		} else if (acc_wedge_count[guess] < r) {
			L = guess + 1;
			++itvl;
		} else {
			R = guess - 1;
			--itvl;
		}
	}
	return res != -1 ? res : itvl;
}

}  // namespace

long double clustering(const Graph& g) {
	const int N = g.num_nodes();
	long long triangles = 0;
	for (int i = 0; i < N; ++i)
		for (int j = i + 1; j < N; ++j)
			for (int k = j + 1; k < N; ++k)
				if (g.is_adjacent(i, j) && g.is_adjacent(j, k) && g.is_adjacent(k, i))
					++triangles;
	return 6.0L * triangles / (static_cast<long double>(N) * (N - 1) * (N - 2));
}

long double uniform_wedge(const Graph& g, int sample_sz, std::mt19937& rng) {
	const int N = g.num_nodes();
	vector<int> acc_wedge_count(N);
	int total_wedges = 0;

	for (int i = 0; i < N; ++i) {
		acc_wedge_count[i] = total_wedges;
		int degree = g.degree(i);
		total_wedges += degree * (degree - 1) / 2;
	}
	if (total_wedges == 0) return 0;

	uniform_int_distribution<int> wedge_pick(0, total_wedges);
	long long sum = 0;

	for (int i = 0; i < sample_sz; ++i) {
		int wedge = wedge_pick(rng);
		int center = binary_search_wedge(N, wedge, acc_wedge_count);

		int degree = g.degree(center);
		if (degree < 2) continue;

		uniform_int_distribution<int> nbr_pick(0, degree - 1);
		int v1 = 0, v2 = 0;
		do {
			v1 = nbr_pick(rng);
			v2 = nbr_pick(rng);
		} while (v1 == v2);

		auto nbrs = g.neighbours_of(center);
		sum += g.is_adjacent(nbrs[v1], nbrs[v2]) ? 1 : 0;
	}

	return sum / (3.0L * sample_sz);
}

double approx_clustering_naive(const Graph& g, int s, std::mt19937& rng) {
	assert(s > 3);

	const int N = g.num_nodes();
	uniform_int_distribution<int> node_pick(0, N - 1);
	int ct = 0;

	for (int i = 0; i < s; ++i) {
		int u = node_pick(rng);
		int degree = g.degree(u);
		if (degree < 2) continue;

		uniform_int_distribution<int> nbr_pick(0, degree - 1);
		int v1 = 0, v2 = 0;
		do {
			v1 = nbr_pick(rng);
			v2 = nbr_pick(rng);
		} while (v1 == v2);

		auto nbrs = g.neighbours_of(u);
		if (g.is_adjacent(nbrs[v1], nbrs[v2])) ++ct;
	}
	return 1.0 * ct / s;
}

}  // namespace graph_approx
