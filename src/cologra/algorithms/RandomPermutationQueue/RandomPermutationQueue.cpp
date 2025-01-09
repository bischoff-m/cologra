#include "RandomPermutationQueue.hpp"
#include "../../Heuristic.hpp"
#include "../../definitions.hpp"
// TODO: Remove before commit and fix
#include "ResultOrException.cpp"
#include "ResultOrException.hpp"
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/mpi.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/serialization/vector.hpp>
#include <fmt/core.h>
#include <random>

using namespace boost;

enum NodeState { IDLE, COMPUTING };

struct ColoringJob {
  Graph graph;
  HeuristicOrder order;

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);
};

template <class Archive>
void ColoringJob::serialize(Archive &ar, const unsigned int version) {
  ar &graph;
  ar &order;
}

RandomPermutationQueue::RandomPermutationQueue(const nlohmann::json &params)
    : ColoringAlgorithm(params, AlgorithmId("RandomPermutationQueue", "1.0")) {
  mpi::communicator world;
  if (world.size() == 1)
    throw invalid_argument("This algorithm is only for parallel execution");

  if (!params.contains("heuristic"))
    throw invalid_argument("Missing heuristic parameter");
  if (!Heuristic::isHeuristic(params["heuristic"]))
    throw invalid_argument("Invalid heuristic parameter");
  heuristicId = params["heuristic"];
}

vector<int> samplePermutationUniform(int size) {
  // Initialize set of available indices
  std::set<int> available;
  for (int i = 0; i < size; i++) {
    available.insert(i);
  }

  vector<int> result;
  random_device randomDevice;
  default_random_engine randomEngine(randomDevice());

  // Sample indices, remove from available set
  while (!available.empty()) {
    uniform_int_distribution<int> uniformDist(0, available.size() - 1);
    int index = uniformDist(randomEngine);
    auto it = available.begin();
    std::advance(it, index);
    int value = *it;
    available.erase(it);
    result.push_back(value);
  }
  return result;
}

VerticesSizeType RandomPermutationQueue::computeColoring(
    Graph graph, ColorMap coloring) {

  mpi::communicator world;
  if (world.rank() != 0) {
    throw invalid_argument("This algorithm assumes that computeColoring is "
                           "only called on the main node");
  }

  HeuristicOrder order = Heuristic::fromId(heuristicId, graph);
  // Segment the order into sections with the same heuristic value
  // First = start (inclusive), second = end (exclusive)
  vector<pair<int, int>> sections;
  int currentHeuristic = order[0].value;
  int start = 0;
  for (int i = 1; i < order.size(); i++) {
    if (order[i].value == currentHeuristic)
      continue;
    sections.push_back({start, i});
    start = i;
    currentHeuristic = order[i].value;
  }
  sections.push_back({start, order.size()});

  int count = 20;
  bool isDone = false;
  VerticesSizeType bestNumColors = numeric_limits<VerticesSizeType>::max();
  vector<ColorType> bestColoring;
  queue<vector<VerticesSizeType>> permutationQueue;

  vector<mpi::request> requests;
  requests.reserve(world.size() - 1);
  // No value = idle, value = result or exception
  vector<ResultOrException> results;
  results.reserve(world.size() - 1);
  vector<NodeState> nodeStates(world.size() - 1, NodeState::IDLE);

  for (int i = 1; i < world.size(); i++) {
    results.push_back(ResultOrException());
    requests.push_back(world.irecv(i, 2, results[i - 1]));
  }

  while (!isDone) {
    // Handle incoming messages
    for (int i = 0; i < requests.size(); i++) {
      if (!requests[i].test()) {
        continue;
      }
      if (results[i].isException()) {
        cout << fmt::format(
                    "Node {} threw exception: {}", i + 1, results[i].exception)
             << endl;
      } else {
        cout << fmt::format("Node {} finished with {} colors",
                    i + 1,
                    results[i].numColors)
             << endl;
        if (results[i].numColors < bestNumColors) {
          bestNumColors = results[i].numColors;
          bestColoring = results[i].coloring;
        }
      }
      results[i] = ResultOrException();
      requests[i] = world.irecv(i + 1, 2, results[i]);
      nodeStates[i] = NodeState::IDLE;
    }

    // Hand out coloring tasks to idle nodes
    for (int i = 1; i < world.size(); i++) {
      if (permutationQueue.empty())
        break;
      if (nodeStates[i - 1] != NodeState::IDLE)
        continue;
      vector<VerticesSizeType> permutation = permutationQueue.front();
      permutationQueue.pop();
      nodeStates[i - 1] = NodeState::COMPUTING;
      mpi::request sendRequest = world.isend(i, 1, ColoringJob{graph, order});
      sendRequest.wait();
    }

    // Stop if total number of permutations is reached
    if (count == 0) {
      isDone = true;
      for (auto &state : nodeStates) {
        if (state != NodeState::IDLE) {
          isDone = false;
          break;
        }
      }
      continue;
    }
    count--;

    // Generate permutation
    vector<VerticesSizeType> permutation;
    for (auto &section : sections) {
      if (section.first == section.second + 1) {
        permutation.push_back(order[section.first].node);
        continue;
      }
      vector<int> sectionPerm =
          samplePermutationUniform(section.second - section.first);
      for (int i = 0; i < sectionPerm.size(); i++) {
        permutation.push_back(order[section.first + sectionPerm[i]].node);
      }
    }
    permutationQueue.push(permutation);
  }

  // Copy best coloring to output
  for (auto it = vertices(graph).first; it != vertices(graph).second; it++) {
    coloring[*it] = bestColoring[*it];
  }
  return bestNumColors;
}

void RandomPermutationQueue::stopIfParallel() {
  mpi::communicator world;
  if (world.rank() != 0)
    return;
  string stopSignal = "stop";
  mpi::request stopRequests[world.size() - 1];
  for (int i = 1; i < world.size(); i++) {
    stopRequests[i - 1] = world.isend(i, 0, stopSignal);
  }
  mpi::wait_all(stopRequests, stopRequests + world.size() - 1);
}

ResultOrException computeColoringOrdered(ColoringJob request) {
  Graph &graph = request.graph;
  HeuristicOrder &order = request.order;
  if (order.size() != num_vertices(graph)) {
    return ResultOrException("Order size does not match number of vertices");
  }

  vector<VerticesSizeType> orderVec(boost::num_vertices(graph));
  ColorMap orderMap(&orderVec.front(), get(vertex_index, graph));
  for (int i = 0; i < order.size(); i++) {
    orderMap[i] = order[i].node;
  }

  try {
    // Initialize empty coloring
    vector<VerticesSizeType> colorVec(boost::num_vertices(graph));
    ColorMap coloring(&colorVec.front(), get(vertex_index, graph));

    // Solve coloring
    VerticesSizeType numColors =
        sequential_vertex_coloring(graph, orderMap, coloring);

    vector<ColorType> coloringVec;
    for (auto it = vertices(graph).first; it != vertices(graph).second; it++) {
      coloringVec.push_back(coloring[*it]);
    }
    return ResultOrException(numColors, coloringVec);
  } catch (const std::exception &e) {
    return ResultOrException(e.what());
  }
}

void RandomPermutationQueue::assistIfParallel() {
  mpi::communicator world;
  if (world.rank() == 0) {
    return;
  }

  string stopSignal;
  ColoringJob job;
  mpi::request stopRequest = world.irecv(0, 0, stopSignal);
  mpi::request computeRequest = world.irecv(0, 1, job);
  while (true) {
    if (stopRequest.test()) {
      computeRequest.cancel();
      break;
    }
    if (computeRequest.test()) {
      ResultOrException result = computeColoringOrdered(job);
      job = ColoringJob();
      computeRequest = world.irecv(0, 1, job);
      mpi::request resultRequest = world.isend(0, 2, result);
      resultRequest.wait();
    }
  }
}
