#!/usr/bin/env bash
# Riondato runtime vs graph size — Fig 7.
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$HERE/.."
BIN="$ROOT/build/release/graph-approx"

mkdir -p "$HERE/results"
OUT="$HERE/results/riondato_runtime.csv"
echo "n,time_ms" > "$OUT"

for n in 1003 2003 3003 4003 5003 6003 7003 8003 9003 10003; do
    echo "  n=$n..."
    GRAPH=$(mktemp)
    TIMING_OUT=$(mktemp)
    "$BIN" generate --n=$n --seed=42 > "$GRAPH"
    "$BIN" riondato --c=0.5 --eps=0.1 --delta=0.1 --seed=42 --timing < "$GRAPH" > /dev/null 2>"$TIMING_OUT"
    TIME_MS=$(grep "elapsed_ms=" "$TIMING_OUT" | sed 's/elapsed_ms=//')
    echo "$n,$TIME_MS" >> "$OUT"
    rm "$GRAPH" "$TIMING_OUT"
done
echo "Output: $OUT"
