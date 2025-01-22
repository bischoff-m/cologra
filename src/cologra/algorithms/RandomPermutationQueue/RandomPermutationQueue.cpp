#include "RandomPermutationQueue.hpp"
#include "../../Heuristic.hpp"
#include "../../definitions.hpp"
#include "../../util/coloring.hpp"
// TODO: Code breaks without this cpp include, why?
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

enum Message : int {
  STOP = 0,
  IDLE = 1,
  GRAPH = 2,
  RESULT = 3,
};

RandomPermutationQueue::RandomPermutationQueue(const nlohmann::json &params)
    : ColoringAlgorithm(params, AlgorithmId("RandomPermutationQueue", "1.1")) {
  mpi::communicator world;
  if (world.size() == 1)
    throw invalid_argument("This algorithm is only for parallel execution");

  if (!params.contains("heuristic"))
    throw invalid_argument("Missing heuristic parameter");
  if (!Heuristic::isHeuristic(params["heuristic"]))
    throw invalid_argument("Invalid heuristic parameter");
  heuristicId = params["heuristic"];

  if (!params.contains("numPermutations"))
    throw invalid_argument("Missing numPermutations parameter");
  if (!params["numPermutations"].is_number_integer())
    throw invalid_argument("numPermutations must be an integer");
  numPermutations = params["numPermutations"];
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

OutType RandomPermutationQueue::computeColoring(Graph graph) {
  mpi::communicator world;
  if (world.rank() != 0) {
    throw invalid_argument("This algorithm assumes that computeColoring is "
                           "only called on the master node");
  }
  // Send graph to all nodes
  for (int i = 1; i < world.size(); i++) {
    world.send(i, Message::GRAPH, graph);
  }

  int count = 0;
  bool isDone = false;
  VerticesSizeType bestNumColors = numeric_limits<VerticesSizeType>::max();
  vector<ColorType> bestColoring;

  vector<ResultOrException> results(world.size() - 1);
  vector<mpi::request> requests(world.size() - 1);
  vector<mpi::request> idleRequests(world.size() - 1);
  vector<bool> isIdle(world.size() - 1, false);

  for (int i = 1; i < world.size(); i++) {
    results[i - 1] = ResultOrException();
    requests[i - 1] = world.irecv(i, Message::RESULT, results[i - 1]);
  }

  auto receiveResult = [&](int i) {
    count++;
    if (results[i].isException()) {
      cout << fmt::format(
                  "Node {} threw exception: {}", i + 1, results[i].exception)
           << endl;
    } else {
      if (results[i].numColors < bestNumColors) {
        bestNumColors = results[i].numColors;
        bestColoring = results[i].coloring;
      }
    }
    results[i] = ResultOrException();
    requests[i] = world.irecv(i + 1, Message::RESULT, results[i]);
  };

  while (!all_of(isIdle.begin(), isIdle.end(), [](bool b) { return b; })) {
    // Handle incoming messages
    auto result = mpi::wait_any(requests.begin(), requests.end());

    int i = result.second - requests.begin();
    if (result.first.tag() != Message::RESULT)
      throw invalid_argument("Unexpected message");

    if (isDone) {
      isIdle[i] = true;
    }
    receiveResult(i);
    if (!isDone && count > numPermutations - world.size()) {
      isDone = true;
      for (int i = 1; i < world.size(); i++) {
        idleRequests[i - 1] = world.isend(i, Message::IDLE);
      }
    }
  }
  // Wait for all nodes to finish
  mpi::wait_all(idleRequests.begin(), idleRequests.end());
  for (auto &request : requests) {
    request.cancel();
  }

  return {bestNumColors, make_unique<ColorVector>(bestColoring)};
}

void RandomPermutationQueue::stopIfParallel() {
  mpi::communicator world;
  if (world.rank() != 0)
    return;
  mpi::request stopRequests[world.size() - 1];
  for (int i = 1; i < world.size(); i++) {
    stopRequests[i - 1] = world.isend(i, Message::STOP);
  }
  mpi::wait_all(stopRequests, stopRequests + world.size() - 1);
}

ResultOrException computeColoringOrdered(
    Graph graph, vector<VerticesSizeType> order) {
  if (order.size() != num_vertices(graph)) {
    return ResultOrException("Order size does not match number of vertices");
  }

  mpi::communicator world;
  auto orderIter = getEmptyColoring(graph, order);
  auto [colorVec, colorIter] = getEmptyColoring(graph);

  try {
    // Solve coloring
    VerticesSizeType numColors =
        sequential_vertex_coloring(graph, *orderIter, *colorIter);

    vector<ColorType> coloringVec;
    for (auto it = vertices(graph).first; it != vertices(graph).second; it++) {
      coloringVec.push_back(get(*colorIter, *it));
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

  Graph graph;
  Graph newGraph;
  HeuristicOrder order;
  vector<pair<int, int>> sections;
  mpi::request stopRequest = world.irecv(0, Message::STOP);
  mpi::request idleRequest = world.irecv(0, Message::IDLE);
  mpi::request computeRequest = world.irecv(0, Message::GRAPH, newGraph);
  vector<mpi::request> requests;

  auto updateGraph = [&]() {
    // Update graph
    graph = newGraph;
    newGraph = Graph();
    computeRequest = world.irecv(0, Message::GRAPH, newGraph);

    // Update sections for new graph
    order = Heuristic::fromId(heuristicId, graph);
    // Segment the order into sections with the same heuristic value
    // First = start (inclusive), second = end (exclusive)
    sections.clear();
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
  };

  computeRequest.wait();
  updateGraph();

  while (true) {
    if (num_vertices(graph) == 0) {
      throw invalid_argument("Graph is empty");
    }
    // Compute new permutation
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

    // Compute coloring and send result
    ResultOrException result = computeColoringOrdered(graph, permutation);
    mpi::request resultRequest = world.isend(0, Message::RESULT, result);
    resultRequest.wait();

    if (stopRequest.test()) {
      computeRequest.cancel();
      break;
    }
    if (idleRequest.test()) {
      vector<mpi::request> waitFor = {stopRequest, computeRequest};
      auto resultWait = mpi::wait_any(waitFor.begin(), waitFor.end());
      if (resultWait.first.tag() == Message::GRAPH) {
        idleRequest = world.irecv(0, Message::IDLE);
        updateGraph();
      } else {
        computeRequest.cancel();
        break;
      }
    }
  }
}
