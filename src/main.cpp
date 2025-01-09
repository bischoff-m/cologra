#include "benchmark/entry.hpp"
#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>

int main(int argc, char **argv) {
  mpi::environment env(argc, argv);
  runBenchmark();
  return 0;
}
