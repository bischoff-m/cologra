#include <stdio.h>
#include <vector>
#include <algorithm>

typedef std::vector<int> Coloring;

class Graph : public std::vector<std::vector<int>>
{
public:
    Graph(std::initializer_list<std::vector<int>> list) : std::vector<std::vector<int>>(list) {}

    /**
     * Sort neighbors of each node in ascending order. This is done to minimize
     * the look-up time for the smallest color not used by any of its neighbors.
     */
    void sortNeighbors()
    {
        for (std::vector<int> &neighbors : *this)
            std::sort(neighbors.begin(), neighbors.end());
    }
};

/**
 * Greedy sequential coloring
 *
 * Reference
 * ---------
 * Page 691 of the paper:
 * Gebremedhin et al. “What Color Is Your Jacobian? Graph Coloring
 * for Computing Derivatives.” SIAM Review 47, no. 4 (January 2005): 629–705.
 * https://doi.org/10.1137/S0036144504444711.
 */
Coloring greedyColoring(Graph graph)
{
    int size = graph.size();
    Coloring coloring(size, -1);

    for (int node = 0; node < size; node++)
    {
        // Find smallest color not used by any of its neighbors
        int color;
        for (color = 0; color < size; color++)
        {
            bool available = true;
            for (int neighbor : graph[node])
                if (coloring[neighbor] == color)
                {
                    available = false;
                    break;
                }
            if (available)
                break;
        }

        // Assign the found color to the node
        coloring[node] = color;
    }

    return coloring;
}

/**
 * Check if the given coloring is a valid distance-1 coloring.
 */
bool isDistance1Coloring(Graph graph, Coloring coloring)
{
    for (int node = 0; node < graph.size(); node++)
    {
        for (int neighbor : graph[node])
            if (coloring[node] == coloring[neighbor])
                return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    Graph graph{
        {2, 3},
        {2},
        {0, 1, 3, 4},
        {0, 2},
        {2},
    };
    graph.sortNeighbors();

    Coloring coloring = greedyColoring(graph);
    for (int node = 0; node < coloring.size(); node++)
        printf("Node %d is colored with %d\n", node, coloring[node]);

    if (isDistance1Coloring(graph, coloring))
        printf("The coloring is a valid distance-1 coloring\n");
    else
        printf("The coloring is not a valid distance-1 coloring\n");

    return 0;
}
