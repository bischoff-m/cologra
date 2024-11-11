#pragma once

#include <boost/graph/adjacency_list.hpp>

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef std::pair<int, int> Edge;

typedef graph_traits<Graph>::vertices_size_type VerticesSizeType;
typedef property_map<Graph, vertex_index_t>::const_type VertexIndexMap;
typedef iterator_property_map<VerticesSizeType *, VertexIndexMap> ColorMap;
