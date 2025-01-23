from pathlib import Path
import traceback
from pydantic import BaseModel


class BenchmarkTarget(BaseModel):
    datasetIds: list[str]
    algorithmIds: list[str]
    graphRepresentation: str
    parameters: dict


class MachineInfo(BaseModel):
    osName: str
    totalMemoryBytes: int
    cpuNames: list[str]
    gpuNames: list[str]
    mpiSize: int


class AggregatedValues(BaseModel):
    totalTimeNs: int
    averageTimeNs: int
    sumNumColors: int


class ExecutionResult(BaseModel):
    duration: int
    numColors: int
    coloring: list[int]
    matrixId: str
    graphSize: int
    didFail: bool


class BenchmarkResult(BaseModel):
    timestamp: str
    aggregated: AggregatedValues
    results: list[ExecutionResult]
    datasetId: str
    algorithmId: str
    parentAlgorithmId: str | None = None
    graphRepresentation: str
    parameters: dict
    machineInfo: MachineInfo


class Benchmark(BaseModel):
    target: BenchmarkTarget
    results: list[BenchmarkResult]

    @classmethod
    def fromFiles(self, targetDir: Path) -> dict[str, "Benchmark"]:
        result = dict()
        for file in targetDir.glob("*.json"):
            try:
                b = Benchmark.model_validate_json(file.read_text())
            except Exception:
                print(f"Failed to parse {file}")
                print(traceback.format_exc())

            result[file.stem] = b
        return result
