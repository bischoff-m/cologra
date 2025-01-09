#include "ResultOrException.hpp"

ResultOrException::ResultOrException(
    VerticesSizeType numColors, vector<ColorType> coloring)
    : numColors(numColors), coloring(coloring) {}

ResultOrException::ResultOrException(string exception) : exception(exception) {}

ResultOrException::ResultOrException() : numColors(0), coloring() {}

bool ResultOrException::isException() const { return !exception.empty(); }

template <class Archive>
void ResultOrException::serialize(Archive &ar, const unsigned int version) {
  ar &numColors;
  ar &coloring;
  ar &exception;
}
