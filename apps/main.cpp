#include "graph_approx/apl.h"
#include "graph_approx/betweenness.h"
#include "graph_approx/clustering.h"
#include "graph_approx/generators.h"
#include "graph_approx/graph.h"
#include "graph_approx/mincut.h"

#include <CLI/CLI.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string>

namespace {

uint32_t resolve_seed(long seed) {
	if (seed >= 0) return static_cast<uint32_t>(seed);
	return static_cast<uint32_t>(
		std::chrono::system_clock::now().time_since_epoch().count());
}

template <typename F>
void time_and_run(bool emit_timing, F&& f) {
	using clock = std::chrono::steady_clock;
	const auto start = clock::now();
	f();
	if (emit_timing) {
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			clock::now() - start).count();
		std::cerr << "elapsed_ms=" << ms << '\n';
	}
}

// Writes one CSV row per HyperANF BFS iteration. Appends to the file so
// multiple invocations (different m values) can accumulate into one CSV.
class AplCsvSink : public graph_approx::AplProgressSink {
public:
	AplCsvSink(std::ostream& out, int m, bool needs_header) : out_(out), m_(m) {
		if (needs_header) out_ << "m,distance,n_d,memory_mb\n";
	}
	void on_iteration(int distance, unsigned long n_d, double memory_mb) override {
		out_ << m_ << ',' << distance << ',' << n_d << ',' << memory_mb << '\n';
	}

private:
	std::ostream& out_;
	int m_;
};

}  // namespace

int main(int argc, char** argv) {
	using namespace graph_approx;

	CLI::App app{"Approximation algorithms for large graph metrics"};
	app.require_subcommand(1);

	long seed = -1;
	bool timing = false;

	int b = 4;
	std::string emit_csv;
	auto* apl_cmd = app.add_subcommand("apl", "HyperANF average path length (reads CSR from stdin)");
	apl_cmd->add_option("--b", b, "log2 of register count (m = 2^b)")->capture_default_str();
	apl_cmd->add_option("--seed", seed, "RNG seed (-1 = non-deterministic)");
	apl_cmd->add_option("--emit-csv", emit_csv,
		"Append per-iteration data to a CSV file (m, distance, n_d, memory_mb)");
	apl_cmd->add_flag("--timing", timing, "Emit elapsed_ms to stderr");

	auto* brandes_cmd = app.add_subcommand("brandes", "Exact betweenness centrality (reads CSR from stdin)");
	brandes_cmd->add_flag("--timing", timing, "Emit elapsed_ms to stderr");

	double c = 0.5, eps = 0.5, delta = 0.8;
	auto* riondato_cmd = app.add_subcommand("riondato", "Riondato sampled betweenness (reads CSR from stdin)");
	riondato_cmd->add_option("--c", c, "constant c")->capture_default_str();
	riondato_cmd->add_option("--eps", eps, "error tolerance epsilon")->capture_default_str();
	riondato_cmd->add_option("--delta", delta, "failure probability delta")->capture_default_str();
	riondato_cmd->add_option("--seed", seed, "RNG seed (-1 = non-deterministic)");
	riondato_cmd->add_flag("--timing", timing, "Emit elapsed_ms to stderr");

	std::string clustering_method = "uniform_wedge";
	int samples = 1000;
	auto* clustering_cmd = app.add_subcommand("clustering", "Clustering coefficient (reads CSR from stdin)");
	clustering_cmd->add_option("--method", clustering_method, "exact | uniform_wedge | naive")
		->check(CLI::IsMember({"exact", "uniform_wedge", "naive"}))
		->capture_default_str();
	clustering_cmd->add_option("--samples", samples, "sample size (approximations only)")->capture_default_str();
	clustering_cmd->add_option("--seed", seed, "RNG seed (-1 = non-deterministic)");
	clustering_cmd->add_flag("--timing", timing, "Emit elapsed_ms to stderr");

	int trials = 1;
	auto* mincut_cmd = app.add_subcommand("mincut", "Karger's randomized min-cut (reads CSR from stdin)");
	mincut_cmd->add_option("--trials", trials, "number of independent contraction rounds")->capture_default_str();
	mincut_cmd->add_option("--seed", seed, "RNG seed (-1 = non-deterministic)");
	mincut_cmd->add_flag("--timing", timing, "Emit elapsed_ms to stderr");

	int n = 1000;
	auto* generate_cmd = app.add_subcommand("generate", "Scale-free graph generator (DMS minimal model; writes CSR to stdout)");
	generate_cmd->add_option("--n", n, "number of nodes")->capture_default_str();
	generate_cmd->add_option("--seed", seed, "RNG seed (-1 = non-deterministic)");

	CLI11_PARSE(app, argc, argv);

	std::mt19937 rng(resolve_seed(seed));

	if (apl_cmd->parsed()) {
		auto g = Graph::from_stream(std::cin);
		std::ofstream csv_out;
		std::unique_ptr<AplCsvSink> sink;
		if (!emit_csv.empty()) {
			const bool needs_header = !std::filesystem::exists(emit_csv);
			csv_out.open(emit_csv, std::ios::app);
			sink = std::make_unique<AplCsvSink>(csv_out, 1 << b, needs_header);
		}
		time_and_run(timing, [&] {
			std::cout << apl(g, b, rng, sink.get()) << '\n';
		});
	} else if (brandes_cmd->parsed()) {
		auto g = Graph::from_stream(std::cin);
		time_and_run(timing, [&] {
			auto bet = brandes(g);
			for (std::size_t i = 0; i < bet.size(); ++i) {
				std::cout << i << '\t' << bet[i] << '\n';
			}
		});
	} else if (riondato_cmd->parsed()) {
		auto g = Graph::from_stream(std::cin);
		time_and_run(timing, [&] {
			auto bet = riondato(g, c, eps, delta, rng);
			for (std::size_t i = 0; i < bet.size(); ++i) {
				std::cout << i << '\t' << bet[i] << '\n';
			}
		});
	} else if (clustering_cmd->parsed()) {
		auto g = Graph::from_stream(std::cin);
		time_and_run(timing, [&] {
			if (clustering_method == "exact") {
				std::cout << clustering(g) << '\n';
			} else if (clustering_method == "uniform_wedge") {
				std::cout << uniform_wedge(g, samples, rng) << '\n';
			} else {
				std::cout << approx_clustering_naive(g, samples, rng) << '\n';
			}
		});
	} else if (mincut_cmd->parsed()) {
		auto g = Graph::from_stream(std::cin);
		time_and_run(timing, [&] {
			std::cout << karger(g, trials, rng) << '\n';
		});
	} else if (generate_cmd->parsed()) {
		Graph g = scale_free(n, rng);
		g.to_stream(std::cout);
	}

	return 0;
}
