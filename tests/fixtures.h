#pragma once

#include "graph_approx/graph.h"

#include <utility>
#include <vector>

// Hand-built CSR fixtures for unit tests. Each `make_*` returns a small graph
// where the structure can be verified by hand.

inline graph_approx::Graph make_empty(int n) {
	std::vector<int> offsets(n + 1, 0);
	return graph_approx::Graph(std::move(offsets), {});
}

// Two nodes, one edge: 0-1.
inline graph_approx::Graph make_K2() {
	return graph_approx::Graph({0, 1, 2}, {1, 0});
}

// Triangle: edges 0-1, 1-2, 0-2.
inline graph_approx::Graph make_triangle() {
	return graph_approx::Graph({0, 2, 4, 6}, {1, 2, 0, 2, 0, 1});
}

// Complete graph K4: 4 nodes, 6 undirected edges.
inline graph_approx::Graph make_K4() {
	return graph_approx::Graph({0, 3, 6, 9, 12},
	                           {1, 2, 3, 0, 2, 3, 0, 1, 3, 0, 1, 2});
}

// 4-cycle: 0-1-2-3-0.
inline graph_approx::Graph make_C4() {
	return graph_approx::Graph({0, 2, 4, 6, 8}, {1, 3, 0, 2, 1, 3, 0, 2});
}

// Bowtie: triangles {0,1,2} and {2,3,4} sharing vertex 2 (5 nodes, 6 edges).
inline graph_approx::Graph make_bowtie() {
	return graph_approx::Graph({0, 2, 4, 8, 10, 12},
	                           {1, 2, 0, 2, 0, 1, 3, 4, 2, 4, 2, 3});
}

// Two triangles {0,1,2} and {3,4,5} joined by bridge edge (2,3).
// Min-cut is 1 (the bridge).
inline graph_approx::Graph make_bridge() {
	return graph_approx::Graph({0, 2, 4, 7, 10, 12, 14},
	                           {1, 2, 0, 2, 0, 1, 3, 2, 4, 5, 3, 5, 3, 4});
}

// Path graph 0-1-2-...-(n-1).
inline graph_approx::Graph make_path(int n) {
	std::vector<int> offsets(n + 1, 0);
	std::vector<int> neighbours;
	for (int i = 0; i < n; ++i) {
		offsets[i] = static_cast<int>(neighbours.size());
		if (i > 0) neighbours.push_back(i - 1);
		if (i < n - 1) neighbours.push_back(i + 1);
	}
	offsets[n] = static_cast<int>(neighbours.size());
	return graph_approx::Graph(std::move(offsets), std::move(neighbours));
}
