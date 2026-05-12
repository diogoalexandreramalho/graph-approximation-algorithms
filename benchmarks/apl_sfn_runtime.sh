#!/usr/bin/env bash
# Sweep HyperANF over the 50 SFNs from the report
# (n = 203, 403, ..., 10003; m in {8, 16, 32}).
# Emits two CSVs:
#   apl_sfn_runtime.csv  — (n, m, apl, time_ms)   for Fig 3
#   apl_sfn_detail.csv   — (n, m, distance, n_d, memory_mb) for Fig 2 peak-mem
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$HERE/.."
BIN="$ROOT/build/release/graph-approx"

mkdir -p "$HERE/results"
OUT="$HERE/results/apl_sfn_runtime.csv"
DETAIL="$HERE/results/apl_sfn_detail.csv"
echo "n,m,apl,time_ms" > "$OUT"
echo "n,m,distance,n_d,memory_mb" > "$DETAIL"

for i in $(seq 1 50); do
    n=$((3 + i * 200))
    GRAPH=$(mktemp)
    "$BIN" generate --n=$n --seed=$i > "$GRAPH"
    for b in 3 4 5; do
        m=$((1 << b))
        TIMING_OUT=$(mktemp)
        DETAIL_TMP=$(mktemp)
        APL=$("$BIN" apl --b=$b --seed=42 --timing --emit-csv "$DETAIL_TMP" < "$GRAPH" 2>"$TIMING_OUT")
        TIME_MS=$(grep "elapsed_ms=" "$TIMING_OUT" | sed 's/elapsed_ms=//')
        echo "$n,$m,$APL,$TIME_MS" >> "$OUT"
        # Strip detail header, prepend n, append to combined detail file.
        tail -n +2 "$DETAIL_TMP" | awk -v n=$n -F',' 'BEGIN{OFS=","} {print n, $0}' >> "$DETAIL"
        rm "$TIMING_OUT" "$DETAIL_TMP"
    done
    rm "$GRAPH"
    if (( i % 10 == 0 )); then echo "  $i/50 done"; fi
done
echo "Output: $OUT, $DETAIL"
