#pragma once

#include "../../definitions.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

class ResultOrException {
private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);

public:
  VerticesSizeType numColors;
  vector<ColorType> coloring;
  string exception;

  ResultOrException(VerticesSizeType numColors, vector<ColorType> coloring);
  ResultOrException(string exception);
  ResultOrException();

  bool isException() const;
};
