
#include "TieBreakSequential.hpp"

#include <functional>
#include <iostream>
#include <queue>

using namespace std;

VerticesSizeType greedyColorOrdered(
    Graph graph, ColorMap coloring, std::queue<int> order) {

  // Set all entries in the coloring to -1
  for (auto node : boost::make_iterator_range(boost::vertices(graph)))
    boost::put(coloring, node, -1);

  // Iterate over all nodes
  VerticesSizeType numColors = 0;
  while (! order.empty()) {
    auto node = order.front();
    order.pop();
    // Get the neighbors of the node
    auto neighbors = boost::adjacent_vertices(node, graph);

    // Get the colors of the neighbors
    vector<VerticesSizeType> neighborColors;
    for (auto neighbor : boost::make_iterator_range(neighbors))
      neighborColors.push_back(coloring[neighbor]);

    // Find the smallest color not used by any of its neighbors
    VerticesSizeType color;
    for (color = 0; color < boost::num_vertices(graph); color++)
      if (find(neighborColors.begin(), neighborColors.end(), color) ==
          neighborColors.end())
        break;

    boost::put(coloring, node, color);
    numColors = max(numColors, color + 1);
  }
  return numColors;
}

TieBreakSequential::TieBreakSequential(const nlohmann::json &params)
    : ColoringAlgorithm(params, AlgorithmId("TieBreakSequential", "1.0")) {
        return;
  if (!params.contains("tiebreak_heuristic"))
    throw invalid_argument("Missing heuristic parameter");
  if (!Heuristic::isHeuristic(params["tiebreak_heuristic"]))
    throw invalid_argument("Invalid heuristic parameter");
  heuristicId = params["heuristic"];
}

VerticesSizeType TieBreakSequential::computeColoring(
    Graph graph, ColorMap coloring) {

        this->graph = graph;
        this->bestColorCount = graph.m_vertices.size();
        this->bestColoring = coloring;

        this->calculateHeuristic(graph);

        Task start = {bestColorCount, {}};
        taskPool.emplace(start);

    while(! this->taskPool.empty()) {
        const auto& nextTask = taskPool.front();
        taskPool.pop();
        this->workOnTask(nextTask);
    }

    for (auto node: boost::make_iterator_range(boost::vertices(graph))) {
        coloring[node] = this->bestColoring[node];
    }
    return this->bestColorCount;
}

void TieBreakSequential::calculateHeuristic(const Graph &graph) {

    this->heuristicValuations.clear();

    auto insertOrUpdate = [this] (int node_id, int degree) {
        if (this->heuristicValuations.find(degree) != this->heuristicValuations.end()) {
            this->heuristicValuations.at(degree).push_back(node_id);
        } else {
            this->heuristicValuations.insert({degree, {node_id}});
        }
    };

    for(const auto &node: boost::make_iterator_range(boost::vertices(graph)) ) {
        insertOrUpdate(node, boost::degree(node, graph));
    }

    printf("Vertex count: %ld, different degree count %ld, max degree: %ld \n", 
                graph.m_vertices.size(), 
                heuristicValuations.size(),
                heuristicValuations.begin()->first);

}

bool TieBreakSequential::isBranchWorth(Task task) {
    if (taskPool.size() > 1000) {
        return false;
    }
    std::queue<int> filled = task.order;

    for(const auto &entry : heuristicValuations) {
        if (entry.first >= task.exploredUntil) {
            continue;
        }
        for (int node : entry.second) {
            filled.emplace(node);
        }
    }


    vector<VerticesSizeType> colorVec(boost::num_vertices(graph));
    ColorMap coloring(&colorVec.front(), get(vertex_index, graph));
    VerticesSizeType valuation = greedyColorOrdered(graph, coloring, filled);

    if (valuation < bestColorCount) {
        bestColorCount = valuation;
        bestColoring = tmpColoring;
        return true;
    }


    // expect branch not getting worse
    return valuation <= bestColorCount;

}

void TieBreakSequential::workOnTask(const Task &task) {

    std::queue<int> taskBaseOrder = task.order;

    int level = task.exploredUntil;
    for (auto &entry: heuristicValuations) {
        if (entry.first >= level) {
            continue;
        }

        level = entry.first;
        if (entry.second.size() == 1) {
            taskBaseOrder.emplace(entry.second.front());
        } else {
            std::vector<int> &degreeShare = entry.second;
            printf("splitting at level %ld, (count: %ld) \n", level, degreeShare.size());
            while(std::next_permutation(degreeShare.begin(), degreeShare.end())) {
                std::queue<int> childTask = taskBaseOrder;
                for(int i: degreeShare) {
                    childTask.emplace(i);
                }
                Task child{level, childTask};
                if (isBranchWorth(child)) {
                    taskPool.emplace(child);
                    //printf("inserting new task (qsize %ld) \n", taskPool.size());
                }
            }
            return;
            // only calculate one permutation level
        }

    }

}