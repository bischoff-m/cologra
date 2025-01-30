from benchmark.benchmark_data import Benchmark
from benchmark.definitions import Paths
import numpy as np


def print_biggest_advantage(benchmark: Benchmark):
    # Find result that has algorithmId == "RandomPermutationQueue"
    our_results = [
        result
        for result in benchmark.results
        if result.algorithmId == "RandomPermutationQueue"
    ]
    assert len(our_results) == 1
    our_results = our_results[0]

    # Boost results
    base_results = [
        result
        for result in benchmark.results
        if result.algorithmId == "BoostSequential"
    ]
    assert len(base_results) == 1
    base_results = base_results[0]

    matrixIds = [result.matrixId for result in our_results.results]
    sizes = np.asarray([result.graphSize for result in our_results.results])
    our_num_colors = np.asarray([result.numColors for result in our_results.results])
    base_num_colors = np.asarray([result.numColors for result in base_results.results])

    faulty = (our_num_colors == 1) & (base_num_colors > 1)
    if np.any(faulty):
        print("Faulty results:")
        for idx in np.where(faulty)[0]:
            print(f"Matrix ID: {matrixIds[idx]}")
            print(f"Graph size: {sizes[idx]}")
            print(f"Boost: {base_num_colors[idx]}")
            print(f"Ours: {our_num_colors[idx]}")
            print()
        raise ValueError("Faulty results found")

    diff = 1 - (base_num_colors - our_num_colors) / sizes
    sorted_idx = np.argsort(diff)
    for idx in sorted_idx[:10]:
        print(f"Matrix ID: {matrixIds[idx]}")
        print(f"Graph size: {sizes[idx]}")
        print(f"Boost: {base_num_colors[idx]}")
        print(f"Ours: {our_num_colors[idx]}")
        print(f"Difference: {1 - diff[idx]:.2%}")
        print()


if __name__ == "__main__":
    benchmarks = Benchmark.fromFiles(Paths.shared / "benchmarks")
    bench = benchmarks["benchmark_2025-01-23_20-34-55_148931"]
    print_biggest_advantage(bench)
