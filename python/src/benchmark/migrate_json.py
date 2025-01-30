import json
from benchmark.benchmark_data import Benchmark
from benchmark.definitions import Paths
from suitesparse.util.ss_index import index_available


def migrate():
    index = index_available()
    index.set_index("id", inplace=True)
    files = (Paths.shared / "benchmarks/until_2025-01-23").glob("*.json")
    for file in files:
        with open(file, "r") as f:
            data: dict = json.load(f)
        data["target"]["graphRepresentation"] = "COLUMN_INTERSECTION"
        for result in data["results"]:
            result["graphRepresentation"] = "COLUMN_INTERSECTION"
            result["machineInfo"]["mpiSize"] = -1
            for exec_result in result["results"]:
                del exec_result["logOut"]
                del exec_result["logErr"]
                exec_result["graphSize"] = int(
                    index.loc[exec_result["matrixId"], "ncols"]
                )
                exec_result["coloring"] = [-1]
        parsed = Benchmark.model_validate(data)
        with open(Paths.shared / "benchmarks" / file.name, "w") as f:
            f.write(parsed.model_dump_json(indent=4))
        print(f"Successfully migrated {file}")


if __name__ == "__main__":
    migrate()
