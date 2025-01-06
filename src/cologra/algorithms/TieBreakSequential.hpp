
#pragma once
#include "../ColoringAlgorithm.hpp"
#include "../Heuristic.hpp"
#include "../definitions.hpp"

#include <vector>
#include <queue>

struct Task {
    int exploredUntil;
    std::queue<int> order;
};

/**
 * Uses a heuristic to order the nodes and then colors them in that order.
 */
class TieBreakSequential : public ColoringAlgorithm {
private:
  string heuristicId;

    Graph graph;

    std::queue<Task> taskPool;

    std::map<int, std::vector<int>, std::greater<int>> heuristicValuations;

    ColorMap bestColoring;
    VerticesSizeType bestColorCount;
    ColorMap tmpColoring;

    void enqueue(Task &&task);
    void workOnTask(const Task &task);
    void calculateHeuristic(const Graph &graph);

    bool isBranchWorth(Task task);

public:
  TieBreakSequential(const nlohmann::json &params);
  VerticesSizeType computeColoring(Graph graph, ColorMap coloring) override;

};
