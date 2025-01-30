import json
import matplotlib.pyplot as plt
from benchmark.benchmark_data import Benchmark, BenchmarkResult
from benchmark.definitions import Paths
from datetime import datetime

import numpy as np


def plot_permut_to_colors(results: list[BenchmarkResult], lines: dict[str, int] = {}):
    """Plot parameters.numPermutations vs aggregated.sumNumColors as a scatter
    plot"""
    x = [result.parameters["numPermutations"] for result in results]
    y = [result.aggregated.sumNumColors for result in results]
    # Group by x and use min as y
    xy = {}
    for ix, iy in zip(x, y):
        if ix not in xy:
            xy[ix] = iy
        else:
            xy[ix] = min(xy[ix], iy)

    plt.figure()
    plt.plot(*zip(*sorted(xy.items())), color=plt.cm.tab10(0))
    plt.scatter(x, y, label="RandomPermutationQueue", s=5)
    plt.xlabel("Number of permutations")
    plt.ylabel("Number of colors")
    for idx, (name, y) in enumerate(lines.items()):
        plt.axhline(y, label=name, color=plt.cm.tab10(idx + 1))
    plt.title("# Permutations vs # Colors (lower is better)")
    plt.grid()
    plt.xscale("log")
    plt.yscale("log")
    plt.legend()
    plt.tight_layout()
    plt.savefig(
        Paths.plots / "permutations_to_colors.png", dpi=300, bbox_inches="tight"
    )
    plt.close()


def plot_num_permutations(results: list[BenchmarkResult]):
    """Plot parameters.numPermutations vs aggregated.sumNumColors as a line
    plot"""
    xy = [
        (result.parameters["numPermutations"], result.aggregated.sumNumColors)
        for result in results
    ]
    xy.sort(key=lambda x: x[0])
    x, y = zip(*xy)
    plt.figure()
    plt.plot(x, y, label="RandomPermutationQueue")
    plt.xscale("log")
    plt.xlabel("Number of permutations")
    plt.ylabel("Number of colors")
    plt.title("# Permutations vs # Colors (lower is better)")
    plt.grid()
    plt.tight_layout()
    plt.savefig(Paths.plots / "num_permutations.pdf")
    plt.close()


def run(benchmarks: dict[str, Benchmark], max_num_colors: int):
    results: list[tuple[str, BenchmarkResult]] = []
    for name, benchmark in benchmarks.items():
        for result in benchmark.results:
            # Parse the date
            # "2025-01-09T15:14:33Z"
            pattern = "%Y-%m-%dT%H:%M:%SZ"
            time = datetime.strptime(result.timestamp, pattern)
            if time < datetime(2025, 1, 11):
                results.append((name, result))

    results_small = [result for _, result in results if result.datasetId == "small_mtx"]

    rpq_results = [
        result
        for result in results_small
        if result.algorithmId == "RandomPermutationQueue"
    ]
    other_results = {
        "Uncompressed": max_num_colors,
        "BoostSequential": float("inf"),
        "OrderedSequential": float("inf"),
    }
    for result in results_small:
        if result.algorithmId in other_results:
            other_results[result.algorithmId] = min(
                other_results[result.algorithmId], result.aggregated.sumNumColors
            )

    plot_permut_to_colors(rpq_results, other_results)

    results = [
        result
        for result in rpq_results
        if result.parameters["heuristic"] == "maxDegreeFirst"
    ]
    plot_num_permutations(results, max_num_colors)


if __name__ == "__main__":
    plt.rc("axes", axisbelow=True)
    (Paths.shared / "plots").mkdir(exist_ok=True)

    with open(Paths.subsets / "subset_small_mtx.json") as f:
        small_mtx = json.load(f)

    ncols = {matrix_id: np.nan for matrix_id in small_mtx}
    with open(Paths.subsets / "index.json") as f:
        index = json.load(f)
    for matrix in index:
        if matrix["id"] in ncols:
            ncols[matrix["id"]] = matrix["ncols"]
    max_num_colors = sum(ncols.values())

    benchmarks = Benchmark.fromFiles(Paths.shared / "benchmarks")
    run(benchmarks, max_num_colors)
