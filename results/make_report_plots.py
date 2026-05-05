#!/usr/bin/env python3
import csv
from collections import defaultdict
from pathlib import Path

import matplotlib.pyplot as plt


def load_rows(csv_path: Path):
    rows = []
    with csv_path.open("r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append(
                {
                    "scenario": row["scenario"],
                    "algorithm": row["algorithm"],
                    "n": int(row["n"]),
                    "k": int(row["k"]),
                    "threads": int(row["threads"]),
                    "distribution": row["distribution"],
                    "avg_ms": float(row["avg_ms"]),
                    "min_ms": float(row["min_ms"]),
                    "max_ms": float(row["max_ms"]),
                }
            )
    return rows


def algo_label(algo: str) -> str:
    names = {
        "full_sort": "Full Sort",
        "heap": "Heap",
        "quickselect": "Quickselect",
        "parallel_heapmerge": "Parallel Heap+Merge",
    }
    return names.get(algo, algo)


def by_scenario(rows):
    bucket = defaultdict(list)
    for r in rows:
        bucket[r["scenario"]].append(r)
    return bucket


def plot_scale_n(rows, out_path: Path):
    scenario = "scale_n_uniform_k100_t4"
    subset = [r for r in rows if r["scenario"] == scenario]
    fig, ax = plt.subplots(figsize=(9.5, 5.5))

    algorithms = sorted(set(r["algorithm"] for r in subset))
    for algo in algorithms:
        pts = sorted([r for r in subset if r["algorithm"] == algo], key=lambda x: x["n"])
        x = [p["n"] for p in pts]
        y = [p["avg_ms"] for p in pts]
        yerr_low = [p["avg_ms"] - p["min_ms"] for p in pts]
        yerr_high = [p["max_ms"] - p["avg_ms"] for p in pts]
        ax.errorbar(
            x,
            y,
            yerr=[yerr_low, yerr_high],
            marker="o",
            linewidth=1.8,
            capsize=3,
            label=algo_label(algo),
        )

    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("Input size n")
    ax.set_ylabel("Runtime (ms, log scale)")
    ax.set_title("Runtime vs Input Size (k=100, threads=4, uniform)")
    ax.grid(True, which="both", linestyle="--", alpha=0.35)
    ax.legend()
    fig.tight_layout()
    fig.savefig(out_path, dpi=220)
    plt.close(fig)


def plot_scale_k(rows, out_path: Path):
    scenario = "scale_k_uniform_n100000_t4"
    subset = [r for r in rows if r["scenario"] == scenario]
    fig, ax = plt.subplots(figsize=(9.5, 5.5))

    algorithms = sorted(set(r["algorithm"] for r in subset))
    for algo in algorithms:
        pts = sorted([r for r in subset if r["algorithm"] == algo], key=lambda x: x["k"])
        x = [p["k"] for p in pts]
        y = [p["avg_ms"] for p in pts]
        ax.plot(x, y, marker="o", linewidth=1.8, label=algo_label(algo))

    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("Top-k")
    ax.set_ylabel("Runtime (ms, log scale)")
    ax.set_title("Runtime vs k (n=100000, threads=4, uniform)")
    ax.grid(True, which="both", linestyle="--", alpha=0.35)
    ax.legend()
    fig.tight_layout()
    fig.savefig(out_path, dpi=220)
    plt.close(fig)


def plot_threads_speedup(rows, out_path: Path):
    scenario = "threads_uniform_n1000000_k100"
    subset = [r for r in rows if r["scenario"] == scenario]

    # Speedup = T(1 thread) / T(p threads)
    baseline = {}
    for r in subset:
        if r["threads"] == 1:
            baseline[r["algorithm"]] = r["avg_ms"]

    fig, ax = plt.subplots(figsize=(9.5, 5.5))
    algorithms = sorted(set(r["algorithm"] for r in subset))
    for algo in algorithms:
        pts = sorted([r for r in subset if r["algorithm"] == algo], key=lambda x: x["threads"])
        x = [p["threads"] for p in pts]
        b = baseline.get(algo, pts[0]["avg_ms"])
        y = [b / p["avg_ms"] for p in pts]
        ax.plot(x, y, marker="o", linewidth=1.8, label=algo_label(algo))

    ax.set_xlabel("Threads")
    ax.set_ylabel("Speedup vs 1 thread")
    ax.set_title("Thread Scaling Speedup (n=1,000,000, k=100, uniform)")
    ax.set_xticks([1, 2, 4, 8])
    ax.grid(True, linestyle="--", alpha=0.35)
    ax.legend()
    fig.tight_layout()
    fig.savefig(out_path, dpi=220)
    plt.close(fig)


def plot_distribution_speedup(rows, out_path: Path):
    scenario = "distribution_n100000_k100_t4"
    subset = [r for r in rows if r["scenario"] == scenario]
    distributions = sorted(set(r["distribution"] for r in subset))

    # Speedup over full sort in same distribution.
    by_dist_algo = defaultdict(dict)
    for r in subset:
        by_dist_algo[r["distribution"]][r["algorithm"]] = r["avg_ms"]

    fig, ax = plt.subplots(figsize=(10.0, 5.8))
    algorithms = ["heap", "quickselect", "parallel_heapmerge"]
    width = 0.25
    x = list(range(len(distributions)))

    for i, algo in enumerate(algorithms):
        vals = []
        for d in distributions:
            full = by_dist_algo[d]["full_sort"]
            vals.append(full / by_dist_algo[d][algo])
        xpos = [xi + (i - 1) * width for xi in x]
        ax.bar(xpos, vals, width=width, label=algo_label(algo))

    ax.set_xticks(x)
    ax.set_xticklabels(distributions, rotation=20, ha="right")
    ax.set_ylabel("Speedup vs Full Sort (x)")
    ax.set_title("Distribution Robustness (n=100000, k=100, threads=4)")
    ax.grid(True, axis="y", linestyle="--", alpha=0.35)
    ax.legend()
    fig.tight_layout()
    fig.savefig(out_path, dpi=220)
    plt.close(fig)


def plot_parallel_wins(rows, out_path: Path):
    # Aggregate parallel gain over heap by experiment family.
    grouped = defaultdict(dict)
    for r in rows:
        key = (r["scenario"], r["n"], r["k"], r["threads"], r["distribution"])
        grouped[key][r["algorithm"]] = r["avg_ms"]

    family_speedups = defaultdict(list)
    for key, algos in grouped.items():
        if "parallel_heapmerge" not in algos or "heap" not in algos:
            continue
        scenario = key[0]
        if scenario.startswith("scale_n"):
            family = "Scale n"
        elif scenario.startswith("scale_k"):
            family = "Scale k"
        elif scenario.startswith("distribution"):
            family = "Distributions"
        elif scenario.startswith("threads"):
            family = "Threads"
        else:
            family = "Other"
        family_speedups[family].append(algos["heap"] / algos["parallel_heapmerge"])

    order = ["Scale n", "Scale k", "Distributions", "Threads"]
    categories = [c for c in order if c in family_speedups]
    means = [sum(family_speedups[c]) / len(family_speedups[c]) for c in categories]
    mins = [min(family_speedups[c]) for c in categories]
    maxs = [max(family_speedups[c]) for c in categories]
    yerr = [[m - lo for m, lo in zip(means, mins)], [hi - m for m, hi in zip(means, maxs)]]

    fig, ax = plt.subplots(figsize=(9.5, 5.5))
    ax.bar(categories, means, yerr=yerr, capsize=6)
    ax.set_ylabel("Heap Runtime / Parallel Runtime (x)")
    ax.set_title("Parallel Advantage Over Heap by Experiment Family")
    ax.grid(True, axis="y", linestyle="--", alpha=0.35)
    fig.tight_layout()
    fig.savefig(out_path, dpi=220)
    plt.close(fig)


def main():
    root = Path(__file__).resolve().parent
    csv_path = root / "benchmark_results.csv"
    out_dir = root / "figures"
    out_dir.mkdir(parents=True, exist_ok=True)

    rows = load_rows(csv_path)
    plot_scale_n(rows, out_dir / "01_runtime_vs_n.png")
    plot_scale_k(rows, out_dir / "02_runtime_vs_k.png")
    plot_threads_speedup(rows, out_dir / "03_thread_scaling_speedup.png")
    plot_distribution_speedup(rows, out_dir / "04_distribution_speedup.png")
    plot_parallel_wins(rows, out_dir / "05_parallel_top_scenarios.png")

    print(f"Wrote figures to: {out_dir}")


if __name__ == "__main__":
    main()
