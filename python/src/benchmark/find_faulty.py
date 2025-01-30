from typing import Callable
from benchmark.benchmark_data import Benchmark
from benchmark.definitions import Paths
import numpy as np


def gather_groud_truth(
    agg_func: Callable[[np.ndarray], np.number] = np.min,
) -> dict[str, int]:
    benchmarks = Benchmark.fromFiles(Paths.shared / "benchmarks")

    num_colors = dict()
    for bench in benchmarks.values():
        for result in bench.results:
            if result.algorithmId != "BoostSequential":
                continue
            for exec_result in result.results:
                mid = exec_result.matrixId
                if mid in num_colors:
                    if (
                        exec_result.numColors == 1
                        and num_colors[mid] > 1
                        or exec_result.numColors > 1
                        and num_colors[mid] == 1
                    ):
                        print(f"Faulty result for {mid}")
                    num_colors[mid] = agg_func([num_colors[mid], exec_result.numColors])
                else:
                    num_colors[mid] = exec_result.numColors
    return num_colors


def find_faulty(truth: dict[str, int]) -> list[str]:
    benchmarks = Benchmark.fromFiles(Paths.shared / "benchmarks")

    flagged = []
    for filename, bench in benchmarks.items():
        num_faulty = 0
        num_missing = 0
        for result in bench.results:
            for exec_result in result.results:
                mid = exec_result.matrixId
                if mid not in truth:
                    print(f"Missing ground truth for {mid}")
                    num_missing += 1
                    continue
                if exec_result.numColors == 1 and truth[mid] > 1:
                    print(f"Faulty result for {mid}")
                    num_faulty += 1
        if num_faulty > 0 or num_missing > 0:
            print(f"{filename}: {num_faulty} faulty, {num_missing} missing.")
        if num_faulty > 0:
            flagged.append(filename)
    return flagged


if __name__ == "__main__":
    truth = gather_groud_truth(np.max)
    flagged = find_faulty(truth)
