#pragma once

#include "../../definitions.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;
using namespace cologra;

class ResultOrException {
private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);

public:
  Vertex numColors;
  vector<Color> coloring;
  string exception;

  ResultOrException(Vertex numColors, vector<Color> coloring);
  ResultOrException(string exception);
  ResultOrException();

  bool isException() const;
};
