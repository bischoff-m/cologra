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

MachineInfo getMachineInfo() {
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
  return {os.name(), memory.total_Bytes(), cpuNames, gpuNames};
}

Benchmark::Benchmark(vector<BenchmarkTarget> targets)
    : targets(targets), loader() {}

void Benchmark::measure(AlgorithmFactory getAlgorithm) {
  boost::mpi::communicator world;

  if (world.rank() == 0) {
    cout << "Starting benchmark" << endl;
    loader.load();
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
        benchmarkResult.parameters = target.parameters;
        benchmarkResult.machineInfo = getMachineInfo();

        int64_t totalDuration = 0;
        int64_t totalNumColors = 0;

        ProgressBarIterator<Graph> progressBar(dataset.begin(), dataset.end());
        while (progressBar.begin() != progressBar.end()) {
          auto &[matrixId, graph] = progressBar.next();
          vector<VerticesSizeType> colorVec(boost::num_vertices(graph));
          ColorMap coloring(&colorVec.front(), get(vertex_index, graph));
          VerticesSizeType numColors = 0;
          int64_t duration = 0;
          bool didFail = false;

          // TODO: Fix capture log when running in parallel
          // CaptureLog *rd = new CaptureLog();
          try {
            auto t1 = high_resolution_clock::now();
            numColors = algorithm->computeColoring(graph).first;
            auto t2 = high_resolution_clock::now();
            duration = duration_cast<nanoseconds>(t2 - t1).count();
          } catch ([[with_stacktrace]] std::exception &e) {
            // TODO: Parallel?
            didFail = true;
            // Write to cerr to avoid interference with the capture
            cerr << e.what() << "\n" << boost::stacktrace::stacktrace() << endl;
          }

          ExecutionResult result;
          result.duration = duration;
          result.numColors = numColors;
          result.matrixId = matrixId;
          result.didFail = didFail;
          // result.logOut = rd->getCout();
          // result.logErr = rd->getCerr();
          result.logOut = "";
          result.logErr = "";
          benchmarkResult.results.push_back(result);

          // delete rd;
          totalDuration += duration;
          totalNumColors += numColors;
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
      delete algorithm;
    }
    if (world.rank() == 0) {
      BenchmarkResult::writeMultiple(results, target);
    }
  }
  if (world.rank() == 0) {
    cout << "Benchmark finished" << endl;
  }
}
