#!/usr/bin/env bash
# Locate cnr-2000.edges and place it at datasets/cnr-2000.edges.
#
# The graph is from the LAW dataset (Università di Milano):
#   https://law.di.unimi.it/webdata/cnr-2000/
# LAW distributes it in their proprietary .graph/.properties binary format,
# not edge-list. This script does not re-derive the conversion; it expects
# the edge-list file to be findable on disk.
#
# Search order:
#   1. CLI argument: $1 (path to cnr-2000.edges)
#   2. $CNR_2000_EDGES environment variable
#   3. ../AAva/algorithms/sets/cnr-2000.edges   (sibling course-project repo)
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
DEST="$HERE/cnr-2000.edges"

if [[ -f "$DEST" ]]; then
    echo "cnr-2000.edges already present at $DEST"
    exit 0
fi

CANDIDATES=()
if [[ $# -gt 0 ]]; then CANDIDATES+=("$1"); fi
if [[ -n "${CNR_2000_EDGES:-}" ]]; then CANDIDATES+=("$CNR_2000_EDGES"); fi
CANDIDATES+=("$HERE/../../AAva/algorithms/sets/cnr-2000.edges")

for src in "${CANDIDATES[@]}"; do
    if [[ -f "$src" ]]; then
        cp "$src" "$DEST"
        echo "Copied from $src"
        echo "       to $DEST"
        exit 0
    fi
done

cat >&2 <<EOF
ERROR: Could not locate cnr-2000.edges.

The cnr-2000 graph is from the LAW dataset:
  https://law.di.unimi.it/webdata/cnr-2000/

It is distributed in LAW's binary .graph format. You need an edge-list file
in the format:
  N 2M
  u v        (sorted by u, each undirected edge listed both ways)

If you have such a file, pass its path as an argument:
  $0 /path/to/cnr-2000.edges

Or set the environment variable:
  export CNR_2000_EDGES=/path/to/cnr-2000.edges
  $0
EOF
exit 1
