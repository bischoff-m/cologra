#include "Benchmark.hpp"
#include "BenchmarkResult.hpp"
#include "CaptureLog.hpp"
#include "ProgressBarIterator.hpp"
#include <boost/mpi.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/stacktrace.hpp>
#include <chrono>
#include <ctime>
#include <hwinfo/hwinfo.h>
#include <iostream>

using namespace std;
using namespace std::chrono;
using namespace cologra;

MachineInfo getMachineInfo() {
  boost::mpi::communicator world;

  vector<string> cpuNames;
  for (const auto &cpu : hwinfo::getAllCPUs()) {
    cpuNames.push_back(cpu.modelName());
  }
  vector<string> gpuNames;
  for (const auto &gpu : hwinfo::getAllGPUs()) {
    gpuNames.push_back(gpu.name());
  }
  hwinfo::OS os;
  hwinfo::Memory memory;
  return {os.name(), memory.total_Bytes(), cpuNames, gpuNames, world.size()};
}

Benchmark::Benchmark(vector<BenchmarkTarget> targets)
    : targets(targets), loader() {
  if (targets.empty()) {
    throw runtime_error("No targets specified for benchmark");
  }
  // Check if all targets have the same graph representation
  GraphRepresentation representation = targets[0].graphRepresentation;
  for (const auto &target : targets) {
    if (target.graphRepresentation != representation) {
      throw runtime_error(
          "Different graph representations is not yet supported");
    }
  }
}

void Benchmark::measure(AlgorithmFactory getAlgorithm) {
  boost::mpi::communicator world;

  if (world.rank() == 0) {
    cout << "Starting benchmark" << endl;

    // Load data for given datasetIds
    vector<string> datasetIds;
    for (const auto &target : targets)
      for (const auto &datasetId : target.datasetIds)
        datasetIds.push_back(datasetId);
    loader.load(datasetIds, targets[0].graphRepresentation);
  }

  for (const auto &target : targets) {
    vector<BenchmarkResult> results;
    for (const auto &algorithmId : target.algorithmIds) {
      auto algorithm = getAlgorithm(algorithmId, target.parameters);
      if (world.rank() != 0) {
        algorithm->assistIfParallel();
        continue;
      }
      for (const auto &datasetId : target.datasetIds) {
        cout << "Running " << algorithmId << " on " << datasetId << endl;
        auto dataset = loader.getDataset(datasetId);

        BenchmarkResult benchmarkResult;
        benchmarkResult.timestampMs =
            duration_cast<milliseconds>(system_clock::now().time_since_epoch())
                .count();
        benchmarkResult.datasetId = datasetId;
        benchmarkResult.algorithmId = algorithmId;
        benchmarkResult.graphRepresentation = target.graphRepresentation;
        benchmarkResult.parameters = target.parameters;
        benchmarkResult.machineInfo = getMachineInfo();

        int64_t totalDuration = 0;
        int64_t totalNumColors = 0;

        ProgressBarIterator<Graph> progressBar(dataset.begin(), dataset.end());
        while (progressBar.begin() != progressBar.end()) {
          auto &[matrixId, graph] = progressBar.next();
          Coloring coloring;
          int64_t duration = 0;
          bool didFail = false;

          try {
            auto t1 = high_resolution_clock::now();
            coloring = algorithm->computeColoring(graph);
            auto t2 = high_resolution_clock::now();
            duration = duration_cast<nanoseconds>(t2 - t1).count();
          } catch (std::exception &e) {
            didFail = true;
            // Write to cerr to avoid interference with the capture
            cerr << "Error during coloring: " << e.what() << endl;
          }

          ExecutionResult result;
          result.duration = duration;
          result.numColors = coloring.first;
          result.coloring = *coloring.second.release();
          result.matrixId = matrixId;
          result.graphSize = num_vertices(graph);
          result.didFail = didFail;
          benchmarkResult.results.push_back(result);

          totalDuration += duration;
          totalNumColors += coloring.first;
        }

        benchmarkResult.aggregated.totalTimeNs = totalDuration;
        benchmarkResult.aggregated.averageTimeNs =
            totalDuration / dataset.size();
        benchmarkResult.aggregated.sumNumColors = totalNumColors;
        results.push_back(benchmarkResult);
      }
      if (world.rank() == 0) {
        algorithm->stopIfParallel();
      }
    }
    if (world.rank() == 0) {
      BenchmarkResult::writeMultiple(results, target);
    }
  }
  if (world.rank() == 0) {
    cout << "Benchmark finished" << endl;
  }
}

Benchmark Benchmark::fromJson(json targetsDefinition) {
  vector<BenchmarkTarget> parsed;
  try {
    for (const auto &target : targetsDefinition["targets"]) {
      vector<string> datasetIds;
      for (const auto &datasetId : target["datasetIds"]) {
        datasetIds.push_back(datasetId);
      }
      vector<string> algorithmIds;
      for (const auto &algorithmId : target["algorithmIds"]) {
        algorithmIds.push_back(algorithmId);
      }
      GraphRepresentation representation =
          representationFromString(target["graphRepresentation"]);
      json params = target["parameters"];
      parsed.push_back({datasetIds, algorithmIds, representation, params});
    }
    return Benchmark(parsed);
  } catch (const std::exception &e) {
    throw runtime_error(
        "Error parsing benchmark definition: " + string(e.what()));
  }
}
