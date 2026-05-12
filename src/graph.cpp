#include "graph_approx/graph.h"

#include <algorithm>
#include <istream>
#include <ostream>
#include <utility>

namespace graph_approx {

Graph::Graph(std::vector<int> offsets, std::vector<int> neighbours)
	: offsets_(std::move(offsets)), neighbours_(std::move(neighbours)) {}

Graph Graph::from_stream(std::istream& in) {
	int N = 0;
	int M = 0;
	in >> N >> M;

	std::vector<int> offsets(N + 1, 0);
	std::vector<int> neighbours(M);

	int previous = 0;
	int shift = 0;
	for (int i = 0; i < M; ++i) {
		int u = 0;
		int v = 0;
		in >> u >> v;
		if (u != previous) {
			for (int j = previous + 1; j <= u; ++j) offsets[j] = shift;
			previous = u;
		}
		neighbours[i] = v;
		++shift;
	}
	for (int j = previous + 1; j <= N; ++j) offsets[j] = shift;

	return Graph(std::move(offsets), std::move(neighbours));
}

bool Graph::is_adjacent(int u, int v) const noexcept {
	if (degree(u) > degree(v)) std::swap(u, v);
	const int end = offsets_[u + 1];
	for (int i = offsets_[u]; i < end; ++i) {
		if (neighbours_[i] == v) return true;
	}
	return false;
}

std::vector<std::pair<int, int>> Graph::edges() const {
	std::vector<std::pair<int, int>> result;
	result.reserve(neighbours_.size() / 2);
	for (int u = 0; u < num_nodes(); ++u) {
		const int end = offsets_[u + 1];
		for (int i = offsets_[u]; i < end; ++i) {
			int v = neighbours_[i];
			if (v > u) result.emplace_back(u, v);
		}
	}
	return result;
}

void Graph::print(std::ostream& out) const {
	for (int n : neighbours_) out << n << ' ';
	out << '\n';
	for (int o : offsets_) out << o << ' ';
	out << '\n';
}

void Graph::to_stream(std::ostream& out) const {
	out << num_nodes() << ' ' << num_edges() << '\n';
	for (int u = 0; u < num_nodes(); ++u) {
		const int end = offsets_[u + 1];
		for (int i = offsets_[u]; i < end; ++i) {
			out << u << ' ' << neighbours_[i] << '\n';
		}
	}
}

}  // namespace graph_approx
