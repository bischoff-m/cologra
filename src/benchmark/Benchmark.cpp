#include "Benchmark.hpp"
#include "BenchmarkResult.hpp"
#include "CaptureLog.hpp"
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
  loader.load();
  cout << "Starting benchmark" << endl;
  for (const auto &target : targets) {
    for (const auto &datasetId : target.datasetIds) {
      auto dataset = loader.getDataset(datasetId);
      for (const auto &algorithmId : target.algorithmIds) {
        cout << "Running " << algorithmId << " on " << datasetId << endl;
        auto algorithm = getAlgorithm(algorithmId, target.parameters);

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

        for (const auto &[matrixId, graph] : dataset) {
          vector<VerticesSizeType> colorVec(boost::num_vertices(graph));
          ColorMap coloring(&colorVec.front(), get(vertex_index, graph));
          VerticesSizeType numColors = 0;
          int64_t duration = 0;
          bool didFail = false;

          CaptureLog *rd = new CaptureLog();
          try {
            auto t1 = high_resolution_clock::now();
            numColors = algorithm->computeColoring(graph, coloring);
            auto t2 = high_resolution_clock::now();
            duration = duration_cast<nanoseconds>(t2 - t1).count();
          } catch ([[with_stacktrace]] std::exception &e) {
            didFail = true;
            // Write to cerr to avoid interference with the capture
            cerr << e.what() << "\n" << boost::stacktrace::stacktrace() << endl;
          }

          ExecutionResult result;
          result.duration = duration;
          result.numColors = numColors;
          result.matrixId = matrixId;
          result.didFail = didFail;
          result.logOut = rd->getCout();
          result.logErr = rd->getCerr();
          benchmarkResult.results.push_back(result);

          delete rd;
          totalDuration += duration;
          totalNumColors += numColors;
        }

        benchmarkResult.aggregated.totalTime = totalDuration;
        benchmarkResult.aggregated.averageTime = totalDuration / dataset.size();
        benchmarkResult.aggregated.sumNumColors = totalNumColors;
        benchmarkResult.writeToFile();
      }
    }
  }
  cout << "Benchmark finished" << endl;
}
