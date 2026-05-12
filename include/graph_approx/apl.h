#pragma once

#include <random>

namespace graph_approx {

class Graph;

// Optional callback invoked once per HyperANF BFS iteration, used by the CLI
// (and benchmarks) to capture data for the report's Figs 1, 2, 4, 5.
//
// The algorithm runs identically whether a sink is supplied or not; the sink
// is observation-only.
struct AplProgressSink {
	virtual ~AplProgressSink() = default;

	// Called after each pass of unions has been applied:
	//   distance        — 0-based iteration index (0 = first BFS layer)
	//   n_d             — cumulative neighbourhood-function value at this layer
	//   temp_memory_mb  — high-water memory of the update buffer, in MB
	//                     (matches AAva's `temp.capacity() * 3 * sizeof(int)` metric)
	virtual void on_iteration(int distance, unsigned long n_d, double temp_memory_mb) = 0;
};

// HyperANF estimate of the average path length.
// b: log2 of the number of HyperLogLog registers per node (m = 2^b).
// sink: optional progress callback (nullptr disables instrumentation).
long double apl(const Graph& g, int b, std::mt19937& rng,
                AplProgressSink* sink = nullptr);

}  // namespace graph_approx
