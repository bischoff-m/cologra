import matplotlib.pyplot as plt
from benchmark.benchmark_data import Benchmark, ExecutionResult
from benchmark.definitions import Paths
import numpy as np


def plot_compression(
    result_base: list[ExecutionResult],
    result_ours: list[ExecutionResult],
    label_base: str,
    label_ours: str,
):
    """Plot efficiency of compression."""
    graph_sizes = np.array([r.graphSize for r in result_base])
    assert np.all(graph_sizes == np.array([r.graphSize for r in result_ours]))
    colors_base = np.array([r.numColors for r in result_base])
    colors_ours = np.array([r.numColors for r in result_ours])
    eff_base = colors_base / graph_sizes
    eff_ours = colors_ours / graph_sizes
    bins = 50
    plt.figure()
    plt.hist(eff_base, bins=bins, label=label_base)
    plt.yscale("log")
    plt.grid()
    plt.savefig(Paths.plots / "compression_efficiency_base.pdf", bbox_inches="tight")
    plt.close()

    plt.figure()
    plt.hist(eff_ours, bins=bins, label=label_ours)
    plt.yscale("log")
    plt.grid()
    plt.savefig(Paths.plots / "compression_efficiency_ours.pdf", bbox_inches="tight")
    plt.close()


def run(benchmarks: dict[str, Benchmark]):
    bench = benchmarks["benchmark_2025-01-23_23-26-16_c3e978_faulty"]
    result_base = next(
        r.results for r in bench.results if r.algorithmId == "BoostSequential"
    )
    result_ours = next(
        r.results for r in bench.results if r.algorithmId == "RandomPermutationQueue"
    )
    results = zip(result_base, result_ours)
    results = [r for r in results if r[0].numColors > 1 and r[1].numColors > 1]
    plot_compression(
        [r[0] for r in results],
        [r[1] for r in results],
        "BoostSequential",
        "RandomPermutationQueue",
    )


if __name__ == "__main__":
    plt.rc("axes", axisbelow=True)
    (Paths.shared / "plots").mkdir(exist_ok=True)
    benchmarks = Benchmark.fromFiles(Paths.shared / "benchmarks")
    run(benchmarks)
