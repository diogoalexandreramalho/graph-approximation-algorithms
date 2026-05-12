#include "graph_approx/apl.h"
#include "graph_approx/betweenness.h"
#include "graph_approx/clustering.h"
#include "graph_approx/graph.h"

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

int main() {
	using namespace graph_approx;

	auto g = Graph::from_stream(std::cin);

	std::cout << "GRAPH IN CSR FORMAT\n";
	g.print(std::cout);
	std::cout << '\n';

	std::mt19937 rng(
		static_cast<std::mt19937::result_type>(
			std::chrono::system_clock::now().time_since_epoch().count()));

	std::cout << "CLUSTERING COEFFICIENT\n";
	// Exact Clustering() is O(N^3) — only feasible on small graphs.
	if (g.num_nodes() <= 1000) {
		std::cout << "exact:        " << clustering(g) << '\n';
	} else {
		std::cout << "exact:        (skipped: N=" << g.num_nodes() << ", O(N^3) infeasible)\n";
	}
	std::cout << "uniform_wedge: " << uniform_wedge(g, 3, rng) << '\n';
	std::cout << "naive_sample:  " << approx_clustering_naive(g, 4, rng) << '\n';
	std::cout << '\n';

	std::cout << "BETWEENNESS CENTRALITY\n";
	auto bet = riondato(g, 0.5, 0.5, 0.8, rng);
	for (std::size_t i = 0; i < bet.size(); ++i) {
		std::cout << "Node " << i << " -> " << bet[i] << '\n';
	}
	std::cout << '\n';

	std::cout << "AVERAGE PATH LENGTH\n";
	for (int b = 3; b <= 6; ++b) {
		std::cout << "m = " << (1 << b) << ": " << apl(g, b, rng) << '\n';
	}
	std::cout << '\n';

	std::cout << "END\n";
}
