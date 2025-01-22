# %%

import matplotlib.pyplot as plt
from benchmark.benchmark_data import Benchmark, BenchmarkResult
from benchmark.definitions import Paths


def plot_permut_to_colors(results: list[BenchmarkResult], lines: dict[str, int] = {}):
    """Plot parameters.numPermutations vs aggregated.sumNumColors as a scatter
    plot"""
    x = [result.parameters["numPermutations"] for result in results]
    y = [result.aggregated.sumNumColors for result in results]
    plt.figure()
    plt.scatter(x, y, label="RandomPermutationQueue")
    plt.xlabel("Number of permutations")
    plt.ylabel("Number of colors")
    for idx, (name, y) in enumerate(lines.items()):
        plt.axhline(y, label=name, color=plt.cm.tab10(idx + 1))
    plt.title("# Permutations vs # Colors (lower is better)")
    plt.grid()
    plt.legend(loc="upper right")
    plt.tight_layout()
    plt.savefig(Paths.plots / "permutations_to_colors.pdf")
    plt.close()


def plot_time_compare(results: list[BenchmarkResult]):
    """Plot aggregated.totalTimeNs as a bar plot"""
    # Order by total time
    results.sort(key=lambda x: x.aggregated.totalTimeNs)
    x = [result.algorithmId for result in results]
    y = [result.aggregated.totalTimeNs / 1e9 for result in results]
    plt.figure(figsize=(10, 5))
    plt.barh(x, y)
    plt.xlabel("Total time (s)")
    plt.title("Total time per algorithm (lower is better)")
    plt.grid()
    plt.tight_layout()
    plt.savefig(Paths.plots / "time_compare.pdf")
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


def main():
    print("This is not implemented yet.")
    return

    plt.rc("axes", axisbelow=True)
    (Paths.shared / "plots").mkdir(exist_ok=True)
    benchmarks = Benchmark.fromFiles(Paths.shared / "benchmarks")

    results: list[tuple[str, BenchmarkResult]] = []
    for name, benchmark in benchmarks.items():
        for result in benchmark.results:
            results.append((name, result))

    results_small = [result for _, result in results if result.datasetId == "small_mtx"]

    rpq_results = [
        result
        for result in results_small
        if result.algorithmId == "RandomPermutationQueue"
    ]
    other_results = {
        "BoostSequential": float("inf"),
        "OrderedSequential": float("inf"),
    }
    for result in results_small:
        if result.algorithmId in other_results:
            other_results[result.algorithmId] = min(
                other_results[result.algorithmId], result.aggregated.sumNumColors
            )

    plot_permut_to_colors(rpq_results, other_results)

    # time_benchmark = benchmarks["benchmark_2025-01-09_19-21-28_e04478"]
    time_benchmark2 = benchmarks["benchmark_2025-01-09_23-37-33_15f06b"]
    # time_small = [
    #     result for result in time_benchmark.results if result.datasetId == "small_mtx"
    # ]
    time_medium = [
        result for result in time_benchmark2.results if result.datasetId == "medium_mtx"
    ]
    plot_time_compare(time_medium)

    results = [
        result
        for result in rpq_results
        if result.parameters["heuristic"] == "maxDegreeFirst"
    ]
    plot_num_permutations(results)
