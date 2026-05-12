#pragma once

#include <iosfwd>
#include <span>
#include <utility>
#include <vector>

namespace graph_approx {

// Compressed Sparse Row representation of an undirected graph.
// For an edge {u,v} we expect both (u,v) and (v,u) to appear in the input.
class Graph {
public:
	Graph(std::vector<int> offsets, std::vector<int> neighbours);

	// Read CSR format from a stream:
	//   N  2M
	//   u  v        (2M lines, sorted by u)
	static Graph from_stream(std::istream& in);

	int num_nodes() const noexcept { return static_cast<int>(offsets_.size()) - 1; }
	int num_edges() const noexcept { return static_cast<int>(neighbours_.size()); }
	int degree(int v) const noexcept { return offsets_[v + 1] - offsets_[v]; }

	std::span<const int> neighbours_of(int v) const noexcept {
		return {neighbours_.data() + offsets_[v], static_cast<std::size_t>(degree(v))};
	}

	// O(min(deg u, deg v)).
	bool is_adjacent(int u, int v) const noexcept;

	// Materialise an edge list with each undirected edge appearing once (u < v).
	std::vector<std::pair<int, int>> edges() const;

	// Debug dump: two lines (neighbours / offsets), space-separated.
	void print(std::ostream& out) const;

	// Write in the input-compatible CSR format ("N 2M\n" header + sorted u v pairs).
	void to_stream(std::ostream& out) const;

private:
	std::vector<int> offsets_;     // size = N + 1
	std::vector<int> neighbours_;  // size = 2M
};

}  // namespace graph_approx
