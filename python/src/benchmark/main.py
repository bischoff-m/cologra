# %%

import matplotlib.pyplot as plt
from benchmark.benchmark_data import Benchmark, BenchmarkResult
from benchmark.definitions import Paths




def main():
    plt.rc("axes", axisbelow=True)
    (Paths.shared / "plots").mkdir(exist_ok=True)
    benchmarks = Benchmark.fromFiles(Paths.shared / "benchmarks")

    
