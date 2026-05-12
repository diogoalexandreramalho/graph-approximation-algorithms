# Graph Approximation Algorithms

[![CI](https://github.com/diogoalexandreramalho/graph-approximation-algorithms/actions/workflows/ci.yml/badge.svg)](https://github.com/diogoalexandreramalho/graph-approximation-algorithms/actions/workflows/ci.yml)

## Project Overview

Many systems are naturally modeled as graphs at massive scale — the web, social networks, citation graphs, road networks. On graphs that large (the cnr-2000 web crawl included here has 325K nodes and 3.2M edges), exact algorithms for classic metrics become infeasible: betweenness centrality is `O(VE)`, exact triangle counting for the clustering coefficient is `O(N³)`.

This project implements four standard approximation algorithms with provable error bounds — HyperANF (average path length), Riondato sampling (betweenness), Bhatia's uniform-wedge (clustering coefficient), and Karger (min-cut) — alongside their exact counterparts where feasible. They share a single CLI with seeded reproducibility, and a benchmark pipeline regenerates the figures from the report on cnr-2000 and 50 synthetic scale-free networks.

## Project Structure

```
apps/        graph-approx CLI binary
src/         algorithm implementations
include/     public headers (graph_approx namespace)
tests/       GoogleTest unit tests
benchmarks/  shell scripts that drive the CLI → CSVs in benchmarks/results/
analysis/    Python plot script → PNG figures in analysis/figures/
datasets/    download.sh + (gitignored) graph data
docs/        original course report PDF
```

## Usage

Build and test:

```bash
cmake --preset=release && cmake --build build/release
ctest --test-dir build/release        # 16/16 deterministic tests pass
```

All algorithms ship as subcommands of one binary, `./build/release/graph-approx`. Each reads a CSR-format graph from stdin (except `generate`, which writes one to stdout). Randomised algorithms take `--seed N` for byte-identical reruns and an optional `--timing` flag that emits `elapsed_ms` to stderr.

```bash
graph-approx apl         --b 4 --seed 42 < graph.txt
            # HyperANF estimate of average path length (m = 2^b registers).

graph-approx brandes                    < graph.txt
            # Exact betweenness centrality (deterministic, O(VE)).

graph-approx riondato    --eps 0.3 --c 0.5 --delta 0.1 --seed 42 < graph.txt
            # Sampled betweenness centrality (Riondato-Kornaropoulos).

graph-approx clustering  --method uniform_wedge --samples 1000 --seed 42 < graph.txt
            # Clustering coefficient. --method ∈ {exact, uniform_wedge, naive}.

graph-approx mincut      --trials 20 --seed 42 < graph.txt
            # Karger's randomised min-cut.

graph-approx generate    --n 10003 --seed 42 > graph.txt
            # DMS scale-free graph generator (writes CSR to stdout).
```

Subcommands compose via stdin/stdout — e.g. `graph-approx generate --n 5003 | graph-approx apl --b 5`.

## Reproducing the report

```bash
./datasets/download.sh                 # locates cnr-2000.edges
./benchmarks/reproduce_report.sh       # ~4 min wall time
cd analysis && python plot_report.py   # writes 7 PNGs to analysis/figures/
```

HyperANF APL on cnr-2000 — published vs this binary, m ∈ {8, 16, 32, 64}:

| m  | Report | This run | Δ%    |
| -- | ------ | -------- | ----- |
| 8  | 25.11  | 25.15    | +0.2% |
| 16 | 37.25  | 38.28    | +2.8% |
| 32 | 61.33  | 62.21    | +1.4% |
| 64 | 116.23 | 107.52   | −7.5% |

All four within HyperLogLog's `1.06/√m` theoretical bound. Three of the report's ten figures (Section 5 Clustering — Figs 8, 9, 10) are not reproduced because the original implementations weren't in either source repository.

## References

- Boldi, Rosa, Vigna — *HyperANF: Approximating the neighbourhood function of very large graphs on a budget* (2010)
- Brandes — *A faster algorithm for betweenness centrality* (2001)
- Riondato, Kornaropoulos — *Fast approximation of betweenness centrality through sampling* (2014)
- Bhatia — *Approximate triangle count and clustering coefficient* (2018)
- Flajolet, Fusy, Gandouet, Meunier — *HyperLogLog: The analysis of a near-optimal cardinality estimation algorithm* (2007)

## Contributors

The original implementation and report was a joint work with my friends Miguel Sena, Joana Alves, Tomás Vieira. The report — including the figures this codebase reproduces — is at [docs/AA_Report_G004.pdf](docs/AA_Report_G004.pdf).
