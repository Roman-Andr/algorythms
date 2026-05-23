#!/usr/bin/env python3
import csv
import sys
from collections import defaultdict
from pathlib import Path

import matplotlib.pyplot as plt

ALGORITHM_LABELS = {
    "standard_quicksort": "Standard QuickSort",
    "standard_mergesort": "Standard MergeSort",
    "string_quicksort": "String QuickSort",
    "string_mergesort_lcp": "String MergeSort LCP",
    "msd_radix_sort": "MSD Radix",
    "msd_radix_quick_sort": "MSD Radix + Quick",
}

DATASET_LABELS = {
    "random": "Random",
    "reverse_sorted": "Reverse sorted",
    "almost_sorted": "Almost sorted",
}


def read_csv(path):
    data = defaultdict(lambda: defaultdict(list))
    with open(path, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        required = {
            "dataset_type",
            "n",
            "algorithm",
            "avg_time_us",
            "avg_char_comparisons",
            "sorted_ok",
        }
        missing = required - set(reader.fieldnames or [])
        if missing:
            raise ValueError(f"CSV is missing columns: {sorted(missing)}")
        for row in reader:
            if row["sorted_ok"] not in {"1", "true", "True"}:
                print(f"Warning: not sorted row: {row}", file=sys.stderr)
            dataset = row["dataset_type"]
            algorithm = row["algorithm"]
            data[dataset][algorithm].append(
                (
                    int(row["n"]),
                    float(row["avg_time_us"]),
                    float(row["avg_char_comparisons"]),
                )
            )
    for dataset in data:
        for algorithm in data[dataset]:
            data[dataset][algorithm].sort(key=lambda x: x[0])
    return data


def plot_all(csv_path, out_path):
    data = read_csv(csv_path)
    datasets = ["random", "reverse_sorted", "almost_sorted"]
    algorithms = list(ALGORITHM_LABELS.keys())

    fig, axes = plt.subplots(3, 2, figsize=(18, 16))
    fig.suptitle("A1: String sorting algorithms benchmark", fontsize=18)

    for row, dataset in enumerate(datasets):
        ax_time = axes[row][0]
        ax_cmp = axes[row][1]

        for algorithm in algorithms:
            points = data.get(dataset, {}).get(algorithm, [])
            if not points:
                continue
            ns = [p[0] for p in points]
            times_ms = [p[1] / 1000.0 for p in points]
            comps = [p[2] for p in points]
            label = ALGORITHM_LABELS.get(algorithm, algorithm)
            ax_time.plot(ns, times_ms, marker="o", linewidth=1.6, markersize=3, label=label)
            ax_cmp.plot(ns, comps, marker="o", linewidth=1.6, markersize=3, label=label)

        dataset_title = DATASET_LABELS.get(dataset, dataset)
        ax_time.set_title(f"{dataset_title}: time")
        ax_time.set_xlabel("n, number of strings")
        ax_time.set_ylabel("Average time, ms")
        ax_time.grid(True, alpha=0.3)

        ax_cmp.set_title(f"{dataset_title}: character comparisons / inspections")
        ax_cmp.set_xlabel("n, number of strings")
        ax_cmp.set_ylabel("Average count")
        ax_cmp.grid(True, alpha=0.3)

    handles, labels = axes[0][0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="lower center", ncol=3, fontsize=11)
    fig.tight_layout(rect=[0, 0.06, 1, 0.96])

    out_path = Path(out_path)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(out_path, dpi=180)
    print(f"Saved {out_path}")


def main():
    csv_path = Path(sys.argv[1]) if len(sys.argv) >= 2 else Path("results.csv")
    out_path = Path(sys.argv[2]) if len(sys.argv) >= 3 else Path("results.png")
    plot_all(csv_path, out_path)


if __name__ == "__main__":
    main()
