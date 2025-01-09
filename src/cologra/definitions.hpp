#pragma once

#include <boost/graph/adjacency_list.hpp>

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef uint64_t VertexType; // anything else (especially 'int') is dangerous
typedef std::pair<VertexType, VertexType> Edge;

typedef graph_traits<Graph>::vertices_size_type VerticesSizeType;
typedef graph_traits<Graph>::vertices_size_type ColorType;
typedef graph_traits<Graph>::degree_size_type DegreeType;
typedef property_map<Graph, vertex_index_t>::const_type VertexIndexMap;
typedef iterator_property_map<ColorType *, VertexIndexMap> ColorMap;
typedef std::pair<graph_traits<Graph>::vertex_iterator,
    graph_traits<Graph>::vertex_iterator>
    VertexIteratorRange;
