#!/usr/bin/env bash
# Reproduces Fig 6 and the page-9 table: Brandes vs Riondato on SFN n=10003,
# sweeping epsilon from 0.1 to 0.7.
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$HERE/.."
BIN="$ROOT/build/release/graph-approx"

mkdir -p "$HERE/results"
GRAPH="$HERE/results/sfn_10003.txt"
BRANDES="$HERE/results/brandes_sfn_10003.tsv"
OUT="$HERE/results/riondato_error.csv"

echo "  generating SFN n=10003..."
"$BIN" generate --n=10003 --seed=42 > "$GRAPH"

echo "  computing exact Brandes ground truth..."
"$BIN" brandes < "$GRAPH" > "$BRANDES"

echo "eps,mean_abs_error,max_abs_error" > "$OUT"

for eps in 0.1 0.15 0.2 0.25 0.3 0.35 0.4 0.45 0.5 0.55 0.6 0.65 0.7; do
    echo "  eps=$eps..."
    RIO=$(mktemp)
    "$BIN" riondato --c=0.5 --eps=$eps --delta=0.1 --seed=42 < "$GRAPH" > "$RIO"
    # Both files: "<index>\t<value>" per line, same node order. Diff column-wise.
    STATS=$(paste "$BRANDES" "$RIO" | awk -F'\t' '
        BEGIN { sum = 0; max = 0; n = 0 }
        { d = ($2 > $4 ? $2 - $4 : $4 - $2); sum += d; if (d > max) max = d; n++ }
        END { printf "%.10f,%.10f", sum / n, max }
    ')
    echo "$eps,$STATS" >> "$OUT"
    rm "$RIO"
done

rm "$GRAPH"
echo "Output: $OUT"
