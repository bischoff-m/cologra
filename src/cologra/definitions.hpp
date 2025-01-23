#pragma once

#include <boost/graph/adjacency_list.hpp>

using namespace boost;

namespace cologra {

typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef graph_traits<Graph>::vertices_size_type Vertex;
typedef std::pair<Vertex, Vertex> Edge;
typedef Vertex Color;
typedef graph_traits<Graph>::degree_size_type Degree;

typedef property_map<Graph, vertex_index_t>::const_type VertexMap;
typedef std::vector<Color> ColorVector;
typedef iterator_property_map<Color *, VertexMap> ColorIterator;
typedef std::pair<Color, std::unique_ptr<ColorVector>> Coloring;

} // namespace cologra
