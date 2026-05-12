#!/usr/bin/env bash
# Run HyperANF on cnr-2000 for m in {8, 16, 32, 64}.
# Emits per-iteration data needed for Figs 1, 4, 5 (and partial Fig 2).
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$HERE/.."
BIN="$ROOT/build/release/graph-approx"
DATA="$ROOT/datasets/cnr-2000.edges"

if [[ ! -f "$DATA" ]]; then
    echo "ERROR: cnr-2000.edges not found at $DATA"
    echo "Run: $ROOT/datasets/download.sh"
    exit 1
fi

mkdir -p "$HERE/results"
OUT="$HERE/results/apl_cnr.csv"
rm -f "$OUT"

for b in 3 4 5 6; do
    m=$((1 << b))
    echo "  m=$m..."
    APL=$("$BIN" apl --b=$b --seed=42 --emit-csv "$OUT" < "$DATA")
    echo "    APL=$APL"
done
echo "Output: $OUT"
