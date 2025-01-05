#pragma once

#include <boost/graph/adjacency_list.hpp>

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef uint64_t VertexType;                                //be very cautious about reducing this to 32-bit - not able to handle more than ~4B nodes...
typedef std::pair<VertexType, VertexType> Edge;

typedef graph_traits<Graph>::degree_size_type DegreeType;
typedef graph_traits<Graph>::vertices_size_type VerticesSizeType;
typedef graph_traits<Graph>::vertices_size_type ColorType;
typedef property_map<Graph, vertex_index_t>::const_type VertexIndexMap;
typedef iterator_property_map<ColorType *, ColorType> ColorMap;
