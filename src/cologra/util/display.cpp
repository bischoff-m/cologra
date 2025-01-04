#include "display.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <fmt/core.h>

using namespace std;
using namespace boost;

struct VertexInfo {
  string style;
  string color;
};

typedef adjacency_list<vecS, vecS, undirectedS, VertexInfo> DisplayGraph;

// Reference:
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

typedef struct {
  double r; // a fraction between 0 and 1
  double g; // a fraction between 0 and 1
  double b; // a fraction between 0 and 1
} RGB;

typedef struct {
  double h; // angle in degrees
  double s; // a fraction between 0 and 1
  double v; // a fraction between 0 and 1
} HSV;

static HSV rgb2hsv(RGB in);
static RGB hsv2rgb(HSV in);

HSV rgb2hsv(RGB in) {
  HSV out;
  double min, max, delta;

  min = in.r < in.g ? in.r : in.g;
  min = min < in.b ? min : in.b;

  max = in.r > in.g ? in.r : in.g;
  max = max > in.b ? max : in.b;

  out.v = max; // v
  delta = max - min;
  if (delta < 0.00001) {
    out.s = 0;
    out.h = 0; // undefined, maybe nan?
    return out;
  }
  if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
    out.s = (delta / max); // s
  } else {
    // if max is 0, then r = g = b = 0
    // s = 0, h is undefined
    out.s = 0.0;
    out.h = NAN; // its now undefined
    return out;
  }
  if (in.r >= max)                 // > is bogus, just keeps compilor happy
    out.h = (in.g - in.b) / delta; // between yellow & magenta
  else if (in.g >= max)
    out.h = 2.0 + (in.b - in.r) / delta; // between cyan & yellow
  else
    out.h = 4.0 + (in.r - in.g) / delta; // between magenta & cyan

  out.h *= 60.0; // degrees

  if (out.h < 0.0)
    out.h += 360.0;

  return out;
}

RGB hsv2rgb(HSV in) {
  double hh, p, q, t, ff;
  long i;
  RGB out;

  if (in.s <= 0.0) { // < is bogus, just shuts up warnings
    out.r = in.v;
    out.g = in.v;
    out.b = in.v;
    return out;
  }
  hh = in.h;
  if (hh >= 360.0)
    hh = 0.0;
  hh /= 60.0;
  i = (long)hh;
  ff = hh - i;
  p = in.v * (1.0 - in.s);
  q = in.v * (1.0 - (in.s * ff));
  t = in.v * (1.0 - (in.s * (1.0 - ff)));

  switch (i) {
  case 0:
    out.r = in.v;
    out.g = t;
    out.b = p;
    break;
  case 1:
    out.r = q;
    out.g = in.v;
    out.b = p;
    break;
  case 2:
    out.r = p;
    out.g = in.v;
    out.b = t;
    break;

  case 3:
    out.r = p;
    out.g = q;
    out.b = in.v;
    break;
  case 4:
    out.r = t;
    out.g = p;
    out.b = in.v;
    break;
  case 5:
  default:
    out.r = in.v;
    out.g = p;
    out.b = q;
    break;
  }
  return out;
}

void graphToDot(std::filesystem::path outfile, Graph graph, ColorMap coloring) {
  // Copy Graph to DisplayGraph
  DisplayGraph displayGraph(num_vertices(graph));
  graph_traits<Graph>::edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
    add_edge(source(*ei, graph), target(*ei, graph), displayGraph);
  }

  const double saturation = 0.5;
  const double value = 0.8;
  // Get highest color value
  double maxColor = 0;
  for (auto node : make_iterator_range(vertices(displayGraph)))
    maxColor = max(maxColor, (double)get(coloring, node));

  // Set vertexInfo for each vertex
  double hueStep = 360.0 / (maxColor + 1);
  for (auto node : make_iterator_range(vertices(displayGraph))) {
    HSV hsv;
    hsv.h = hueStep * get(coloring, node);
    hsv.s = saturation;
    hsv.v = value;
    RGB rgb = hsv2rgb(hsv);
    displayGraph[node].color = fmt::format("#{:02x}{:02x}{:02x}",
        (int)(rgb.r * 255),
        (int)(rgb.g * 255),
        (int)(rgb.b * 255));
    displayGraph[node].style = "filled";
  }

  dynamic_properties properties;
  properties.property("style", get(&VertexInfo::style, displayGraph));
  properties.property("color", get(&VertexInfo::color, displayGraph));
  properties.property("node_id", get(vertex_index, displayGraph));

  // Write the graph to a DOT file
  ofstream dotfile(outfile.string());
  write_graphviz_dp(dotfile, displayGraph, properties);
  dotfile.close();
}
