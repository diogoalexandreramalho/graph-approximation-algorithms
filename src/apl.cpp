#include "graph_approx/apl.h"
#include "graph_approx/graph.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace graph_approx {
using namespace std;

namespace {

long jenkins(long x, long seed) {
	unsigned long a, b, c;

	a = seed + x;
	b = seed;
	c = 0x9e3779b97f4a7c13L;  // golden ratio, arbitrary

	a -= b; a -= c; a ^= (c >> 43);
	b -= c; b -= a; b ^= (a << 9);
	c -= a; c -= b; c ^= (b >> 8);
	a -= b; a -= c; a ^= (c >> 38);
	b -= c; b -= a; b ^= (a << 23);
	c -= a; c -= b; c ^= (b >> 5);
	a -= b; a -= c; a ^= (c >> 35);
	b -= c; b -= a; b ^= (a << 49);
	c -= a; c -= b; c ^= (b >> 11);
	a -= b; a -= c; a ^= (c >> 12);
	b -= c; b -= a; b ^= (a << 18);
	c -= a; c -= b; c ^= (b >> 22);

	return c;
}

int count_leading_zeros(int64_t x) {
	const int total_bits = sizeof(x) * 8;
	int res = 0;
	while (!(x & (static_cast<uint64_t>(1) << (total_bits - 1)))) {
		x <<= 1;
		++res;
	}
	return res;
}

double compute_alpha(uint64_t m) {
	switch (m) {
		case 16: return 0.673;
		case 32: return 0.697;
		case 64: return 0.709;
		default: return 0.7213 / (1.0 + 1.079 / m);
	}
}

void hll_add(vector<vector<int>>& c, long v, int b, long seed) {
	int64_t x = jenkins(v, seed);
	int j = static_cast<uint64_t>(x) >> (64 - b);
	int w = count_leading_zeros(x << b) + 1;
	c[v][j] = max(w, c[v][j]);
}

uint64_t hll_size(const vector<vector<int>>& c, uint64_t m, double alpha, long v) {
	long double denominator = 0;
	for (uint64_t i = 0; i < m; ++i) {
		denominator += pow(2, -c[v][i]);
	}
	long double z = 1 / denominator;
	return static_cast<uint64_t>(alpha * pow(m, 2) * z);
}

// NOTE: preserves the original semantics — `entry` is declared once outside
// the loop, so successive register-updates within the same call accumulate
// into the same vector and only the first triple is later applied by
// apply_updates(). This is faithful to the original course implementation.
void hll_union(const vector<vector<int>>& c, long u, long v, uint64_t m,
               vector<vector<int>>& temp, bool& changed) {
	vector<int> entry;
	for (uint64_t i = 0; i < m; ++i) {
		if (c[v][i] > c[u][i]) {
			changed = true;
			entry.push_back(static_cast<int>(u));
			entry.push_back(static_cast<int>(i));
			entry.push_back(c[v][i]);
			temp.push_back(entry);
		}
	}
}

void apply_updates(vector<vector<int>>& c, const vector<vector<int>>& temp) {
	for (const auto& entry : temp) {
		int v = entry[0];
		int n = entry[1];
		int value = entry[2];
		if (value > c[v][n]) c[v][n] = value;
	}
}

void compute_neighbourhood_function(const vector<vector<int>>& c,
                                    vector<unsigned long>& N_function,
                                    uint64_t m, double alpha, int N) {
	uint64_t total = 0;
	for (int v = 0; v < N; ++v) {
		total += hll_size(c, m, alpha, v);
	}
	N_function.push_back(total);
}

long double compute_average_distance(int max_distance,
                                     const vector<unsigned long>& N_function) {
	long double average = 0.0;
	for (int i = 0; i < max_distance - 1; ++i) {
		unsigned long diff = N_function[i + 1] - N_function[i];
		average += diff * (i + 1);
	}
	average /= N_function[max_distance - 1];
	return average;
}

}  // namespace

long double apl(const Graph& g, int b, std::mt19937& rng) {
	const int N = g.num_nodes();
	const uint64_t m = static_cast<uint64_t>(1) << b;
	const long seed = static_cast<long>(rng());
	const double alpha = compute_alpha(m);

	vector<vector<int>> c(N, vector<int>(m, 0));
	for (int v = 0; v < N; ++v) {
		hll_add(c, v, b, seed);
	}

	vector<unsigned long> N_function;
	vector<vector<int>> temp;
	int t = 0;
	bool changed = true;
	while (changed) {
		changed = false;
		temp.clear();
		vector<vector<int>>(temp).swap(temp);

		for (int v = 0; v < N; ++v) {
			vector<int> a = c[v];  // unused copy, preserved from original
			(void)a;
			for (int w : g.neighbours_of(v)) {
				hll_union(c, v, w, m, temp, changed);  // B(v, t+1)
			}
		}

		apply_updates(c, temp);
		compute_neighbourhood_function(c, N_function, m, alpha, N);
		t += 1;
	}
	(void)t;

	const int max_distance = static_cast<int>(N_function.size());
	return compute_average_distance(max_distance, N_function);
}

}  // namespace graph_approx
