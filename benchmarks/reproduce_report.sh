#!/usr/bin/env bash
# Top-level orchestrator: runs all four experiment scripts in order.
# Total wall time ~10-30 min on a modern laptop. Requires:
#   - the binary built at build/release/graph-approx
#   - datasets/cnr-2000.edges in place (run datasets/download.sh)
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"

echo "[1/4] APL on cnr-2000 (Figs 1, 2-partial, 4, 5)..."
bash "$HERE/apl_cnr.sh"

echo "[2/4] APL runtime sweep on 50 SFNs (Fig 3, full Fig 2 data)..."
bash "$HERE/apl_sfn_runtime.sh"

echo "[3/4] Riondato error sweep on SFN n=10003 (Fig 6)..."
bash "$HERE/riondato_error.sh"

echo "[4/4] Riondato runtime sweep (Fig 7)..."
bash "$HERE/riondato_runtime.sh"

echo ""
echo "Done. CSVs in $HERE/results/"
echo "Generate figures: cd analysis && uv run python plot_report.py"
