#include "graph_approx/betweenness.h"
#include "graph_approx/graph.h"

#include <cassert>
#include <cmath>
#include <queue>
#include <random>
#include <stack>
#include <utility>
#include <vector>

namespace graph_approx {
using namespace std;

namespace {

pair<vector<vector<int>>, vector<int>> all_shortest_paths(const Graph& g, int source, int target) {
	const int N = g.num_nodes();

	queue<int> q;
	q.push(source);

	vector<int> sigma_u(N, 0);   sigma_u[source]  = 1;
	vector<int> distance(N, -1); distance[source] = 0;
	vector<bool> enqueued(N, false); enqueued[source] = true;
	vector<vector<int>> predecessors(N);

	while (q.front() != target) {
		int u = q.front(); q.pop();
		enqueued[u] = false;

		for (int w : g.neighbours_of(u)) {
			if (distance[w] == -1) distance[w] = distance[u] + 1;
			if (distance[w] == distance[u] + 1) {
				predecessors[w].push_back(u);
				if (!enqueued[w]) {
					q.push(w);
					enqueued[w] = true;
				}
				sigma_u[w] += sigma_u[u];
			}
		}
	}
	return {predecessors, sigma_u};
}

}  // namespace

vector<double> brandes(const Graph& g) {
	const int N = g.num_nodes();
	vector<double> c(N, 0);

	for (int s = 0; s < N; ++s) {
		queue<int> Q;
		stack<int> S;
		vector<int> distance(N, -1); distance[s] = 0;
		vector<vector<int>> predecessors(N);
		vector<double> sigma(N, 0); sigma[s] = 1;

		Q.push(s);
		while (!Q.empty()) {
			int v = Q.front(); Q.pop();
			S.push(v);
			for (int w : g.neighbours_of(v)) {
				if (distance[w] < 0) {
					Q.push(w);
					distance[w] = distance[v] + 1;
				}
				if (distance[w] == distance[v] + 1) {
					sigma[w] = sigma[w] + sigma[v];
					predecessors[w].push_back(v);
				}
			}
		}

		vector<double> delta(N, 0);
		while (!S.empty()) {
			int w = S.top(); S.pop();
			for (int v : predecessors[w]) {
				delta[v] = delta[v] + sigma[v] / sigma[w] * (1 + delta[w]);
			}
			if (w != s) c[w] = c[w] + delta[w];
		}
	}

	for (int i = 0; i < N; ++i) {
		c[i] = c[i] / ((N - 1) * (N - 2));  // undirected normalisation
	}
	return c;
}

int vertex_diameter_2approx(const Graph& g, std::mt19937& rng) {
	const int N = g.num_nodes();
	uniform_int_distribution<int> pick(0, N - 1);
	int v = pick(rng);

	queue<int> q;
	q.push(v);
	vector<bool> visited(N, false); visited[v] = true;
	vector<int> distance(N, 0);
	int s = 0, s1 = 0, s2 = 0;

	while (!q.empty()) {
		int u = q.front(); q.pop();
		for (int w : g.neighbours_of(u)) {
			if (!visited[w]) {
				s = distance[w] = distance[u] + 1;
				visited[w] = true;
				q.push(w);
				if (s > s1) { s2 = s1; s1 = s; }
				else if (s > s2) s2 = s;
			}
		}
	}
	return s1 + s2;
}

vector<double> riondato(const Graph& g, double c, double eps, double delta, std::mt19937& rng) {
	assert(c >= 0 && c <= 1 && eps >= 0 && eps <= 1);

	const int N = g.num_nodes();
	vector<double> betweenness(N, 0);

	const int diameter = vertex_diameter_2approx(g, rng);
	if (diameter < 2) return betweenness;

	const double r = (c / (eps * eps)) * (floor(log2(diameter - 2)) + log(1 / delta));
	uniform_int_distribution<int> pick(0, N - 1);

	for (int i = 0; i < r; ++i) {
		int source = 0, target = 0;
		do {
			source = pick(rng);
			target = pick(rng);
		} while (source == target);

		auto [predecessors, sigma_u] = all_shortest_paths(g, source, target);

		int t = target;
		while (t != source) {
			int parents = static_cast<int>(predecessors[t].size());
			vector<double> weights(parents);
			for (int j = 0; j < parents; ++j) {
				weights[j] = 1.0 * sigma_u[predecessors[t][j]] / sigma_u[t];
			}
			discrete_distribution<int> distribution(weights.begin(), weights.end());
			int z = predecessors[t][distribution(rng)];

			if (z != source) betweenness[z] += 1.0 / r;
			t = z;
		}
	}

	return betweenness;
}

}  // namespace graph_approx
