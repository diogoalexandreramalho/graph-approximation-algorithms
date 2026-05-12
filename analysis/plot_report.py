"""Generate the report's figures from the CSVs in benchmarks/results/.

Run from the analysis/ directory:
    uv sync
    uv run python plot_report.py

Reads:  ../benchmarks/results/*.csv
Writes: ./figures/*.png
"""
from __future__ import annotations

import math
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd

HERE = Path(__file__).resolve().parent
RESULTS = HERE.parent / "benchmarks" / "results"
FIGURES = HERE / "figures"
FIGURES.mkdir(exist_ok=True)


def fig1_memory_per_depth() -> None:
    csv = RESULTS / "apl_cnr.csv"
    if not csv.exists():
        print(f"  skipping fig 1 — {csv} not present")
        return
    df = pd.read_csv(csv)
    plt.figure(figsize=(7, 4))
    for m, group in df.groupby("m"):
        plt.plot(group["distance"], group["memory_mb"], label=f"m = {m}")
    plt.title("Fig 1 — Memory per BFS depth (cnr-2000)")
    plt.xlabel("Distance")
    plt.ylabel("Memory (MB)")
    plt.legend()
    plt.tight_layout()
    out = FIGURES / "fig1_memory_per_depth.png"
    plt.savefig(out, dpi=100)
    plt.close()
    print(f"  -> {out}")


def fig2_memory_vs_E_m() -> None:
    """Reproduces the report's Fig 2 (page 4):
        total_memory = 2 * (V*m*4 + peak_temp_capacity*3*4 + max_distance*8) bytes
        upper_bound  = 2 * (V*m*4 + 3*E*m*4 + V*8)                          bytes
    Plotted in Kbytes, x-axis is E*m.
    """
    csv = RESULTS / "apl_sfn_detail.csv"
    if not csv.exists():
        print(f"  skipping fig 2 — {csv} not present")
        return
    df = pd.read_csv(csv)

    # Per (n, m): peak temp memory and max BFS depth reached.
    agg = df.groupby(["n", "m"]).agg(
        peak_temp_mb=("memory_mb", "max"),
        max_distance=("distance", "max"),
    ).reset_index()

    # DMS scale-free directed edges: 6 initial + 4 per added node.
    agg["E"] = 6 + 4 * (agg["n"] - 3)
    agg["E_m"] = agg["E"] * agg["m"]

    # Measured total memory (Kbytes), with the report's ×2 reallocation factor.
    counter_bytes = agg["n"] * agg["m"] * 4
    temp_bytes = agg["peak_temp_mb"] * (2 ** 20)
    nd_bytes = (agg["max_distance"] + 1) * 8
    agg["measured_kb"] = 2 * (counter_bytes + temp_bytes + nd_bytes) / 1024

    # Theoretical upper bound: f(V, m, E) = 2*(V*m*4 + 3*E*m*4 + V*8) bytes.
    agg["bound_kb"] = 2 * (counter_bytes + 3 * agg["E"] * agg["m"] * 4 + agg["n"] * 8) / 1024

    plt.figure(figsize=(7, 4))
    bound_sorted = agg.sort_values("E_m")
    plt.plot(bound_sorted["E_m"], bound_sorted["bound_kb"], color="red",
             label="Upper bound — Memory = 2(4Vm + 12Em + 8V)")
    plt.scatter(agg["E_m"], agg["measured_kb"], s=10,
                label="Memory usage of graph with E edges and m registers")
    plt.title("Fig 2 — Memory usage given number of edges and registers")
    plt.xlabel("Number of edges * number of registers (E*m)")
    plt.ylabel("Memory (Kbytes)")
    plt.legend()
    plt.tight_layout()
    out = FIGURES / "fig2_memory_vs_Em.png"
    plt.savefig(out, dpi=100)
    plt.close()
    print(f"  -> {out}")


def fig3_runtime_vs_E_logV() -> None:
    csv = RESULTS / "apl_sfn_runtime.csv"
    if not csv.exists():
        print(f"  skipping fig 3 — {csv} not present")
        return
    df = pd.read_csv(csv)
    df["E"] = 6 + 4 * (df["n"] - 3)
    df["E_logV"] = df["E"] * df["n"].apply(math.log)
    plt.figure(figsize=(7, 4))
    for m, group in df.groupby("m"):
        plt.scatter(group["E_logV"], group["time_ms"], s=10, label=f"m = {m}")
    plt.title("Fig 3 — APL runtime vs E · log V (50 SFNs)")
    plt.xlabel("E · log V")
    plt.ylabel("Time (ms)")
    plt.legend()
    plt.tight_layout()
    out = FIGURES / "fig3_runtime_vs_ElogV.png"
    plt.savefig(out, dpi=100)
    plt.close()
    print(f"  -> {out}")


def fig4_5_cumulative_pmf() -> None:
    csv = RESULTS / "apl_cnr.csv"
    if not csv.exists():
        print(f"  skipping figs 4/5 — {csv} not present")
        return
    df = pd.read_csv(csv)

    plt.figure(figsize=(7, 4))
    for m, group in df.groupby("m"):
        g = group.sort_values("distance")
        plt.plot(g["distance"], g["n_d"], label=f"m = {m}")
    plt.title("Fig 4 — Cumulative N(d) on cnr-2000")
    plt.xlabel("Distance")
    plt.ylabel("Number of pairs")
    plt.legend()
    plt.tight_layout()
    out4 = FIGURES / "fig4_cumulative_Nd.png"
    plt.savefig(out4, dpi=100)
    plt.close()
    print(f"  -> {out4}")

    plt.figure(figsize=(7, 4))
    apl_per_m: dict[int, float] = {}
    for m, group in df.groupby("m"):
        g = group.sort_values("distance").reset_index(drop=True)
        diffs = g["n_d"].diff().fillna(g["n_d"].iloc[0])
        total = float(g["n_d"].iloc[-1])
        if total == 0:
            continue
        pmf = diffs / total
        plt.plot(g["distance"], pmf, label=f"m = {m}")
        apl_per_m[int(m)] = float((pmf * g["distance"]).sum())
    plt.title("Fig 5 — PMF of N(d) on cnr-2000")
    plt.xlabel("Distance")
    plt.ylabel("Probability")
    plt.legend()
    plt.tight_layout()
    out5 = FIGURES / "fig5_pmf_Nd.png"
    plt.savefig(out5, dpi=100)
    plt.close()
    print(f"  -> {out5}")
    print("  APL per m (cnr-2000, this run):")
    for m, apl in sorted(apl_per_m.items()):
        print(f"    m={m:>3}: {apl:.4f}")


def fig6_riondato_error() -> None:
    csv = RESULTS / "riondato_error.csv"
    if not csv.exists():
        print(f"  skipping fig 6 — {csv} not present")
        return
    df = pd.read_csv(csv)
    plt.figure(figsize=(7, 4))
    plt.scatter(df["eps"], df["mean_abs_error"], s=20)
    plt.title("Fig 6 — Riondato betweenness error vs ε (SFN n=10003)")
    plt.xlabel("ε")
    plt.ylabel("Mean absolute error")
    plt.tight_layout()
    out = FIGURES / "fig6_riondato_error_vs_eps.png"
    plt.savefig(out, dpi=100)
    plt.close()
    print(f"  -> {out}")
    print("  Table — max absolute error per ε:")
    for _, row in df.iterrows():
        print(f"    ε={row['eps']:.2f}  max_abs_error={row['max_abs_error']:.6f}")


def fig7_riondato_runtime() -> None:
    csv = RESULTS / "riondato_runtime.csv"
    if not csv.exists():
        print(f"  skipping fig 7 — {csv} not present")
        return
    df = pd.read_csv(csv)
    df["E"] = 6 + 4 * (df["n"] - 3)
    plt.figure(figsize=(7, 4))
    plt.scatter(df["E"], df["time_ms"], s=30)
    plt.title("Fig 7 — Riondato runtime vs E (SFN sweep)")
    plt.xlabel("E (directed edges)")
    plt.ylabel("Time (ms)")
    plt.tight_layout()
    out = FIGURES / "fig7_riondato_runtime.png"
    plt.savefig(out, dpi=100)
    plt.close()
    print(f"  -> {out}")


def main() -> None:
    print(f"Generating figures from {RESULTS}")
    fig1_memory_per_depth()
    fig2_memory_vs_E_m()
    fig3_runtime_vs_E_logV()
    fig4_5_cumulative_pmf()
    fig6_riondato_error()
    fig7_riondato_runtime()
    print(f"Done. Figures in {FIGURES}/")


if __name__ == "__main__":
    main()
