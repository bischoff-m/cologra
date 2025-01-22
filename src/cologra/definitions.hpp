#pragma once

#include <boost/graph/adjacency_list.hpp>

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef uint64_t VertexType; // anything else (especially 'int') is dangerous
typedef std::pair<VertexType, VertexType> Edge;

// Replace by VertexType and rename to Vertex
typedef graph_traits<Graph>::vertices_size_type VerticesSizeType;
// Rename to Color
typedef graph_traits<Graph>::vertices_size_type ColorType;
typedef graph_traits<Graph>::degree_size_type DegreeType;
typedef property_map<Graph, vertex_index_t>::const_type VertexIndexMap;
typedef std::vector<ColorType> ColorVector;
typedef iterator_property_map<ColorType *, VertexIndexMap> ColorIterator;
typedef std::pair<graph_traits<Graph>::vertex_iterator,
    graph_traits<Graph>::vertex_iterator>
    VertexIteratorRange;
// TODO: Rename OutType to Coloring
typedef std::pair<ColorType, std::unique_ptr<ColorVector>> OutType;

// typedef graph_traits<Graph>::vertices_size_type Vertex;
// typedef Vertex Color;
