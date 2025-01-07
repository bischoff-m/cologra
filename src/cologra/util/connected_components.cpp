#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include "../definitions.hpp"

std::vector<Graph> get_connected_components(Graph graph){
    std::vector<VertexType> component (boost::num_vertices(graph));
    VerticesSizeType num_components = boost::connected_components(graph, &component[0]);
    
    std::vector<Graph> res(num_components, boost::num_vertices(graph));
    for (auto e : boost::make_iterator_range(boost::edges(graph))){
        if (component[boost::source(e, graph)] == component[boost::target(e, graph)]){
            boost::add_edge(boost::source(e, graph), boost::target(e, graph), res[component[boost::source(e, graph)]]);
        }
    }
    // do we actually need this part with adjacency lists?
    for (Graph & g : res){
        for (VertexType n : boost::make_iterator_range(boost::vertices(g))){
            if (boost::degree(n,g) == 0){
                boost::remove_vertex(n,g);
            }
        }
    }
    return res;
}
