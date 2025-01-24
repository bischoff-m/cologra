import matplotlib.pyplot as plt
from benchmark.benchmark_data import Benchmark, BenchmarkResult
from benchmark.definitions import Paths


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


def run(benchmarks: dict[str, Benchmark]):
    # time_benchmark = benchmarks["benchmark_2025-01-09_19-21-28_e04478"]
    time_benchmark2 = benchmarks["benchmark_2025-01-09_23-37-33_15f06b"]
    # time_small = [
    #     result for result in time_benchmark.results if result.datasetId == "small_mtx"
    # ]
    time_medium = [
        result for result in time_benchmark2.results if result.datasetId == "medium_mtx"
    ]
    plot_time_compare(time_medium)
