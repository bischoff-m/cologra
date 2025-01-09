#include <argh.h>
#include <iostream>
#include <Eigen/SparseCore>
#include <filesystem>
#include "cologra/definitions.hpp"
#include "cologra/util/matrixIO.hpp"
#include "cologra/util/matrixToGraph.hpp"
#include "cologra/algorithms/BasicSequential.hpp"
#include "cologra/algorithms/BasicParallel.hpp"
#include "cologra/algorithms/OrderedSequential.hpp"
#include "cologra/algorithms/BoostSequential.hpp"
#include "cologra/algorithms/RandomPermutationQueue/RandomPermutationQueue.hpp"
#include <nlohmann/json.hpp>
#include "cologra/ColoringAlgorithm.hpp"
#include <fmt/core.h>
#include "benchmark/entry.hpp"
#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>


ColoringAlgorithm *createAlgorithm(string id, json params) {
  if (id == "BasicSequential") {
    return new BasicSequential();
  } else if (id == "BoostSequential") {
    return new BoostSequential();
  } else if (id == "OrderedSequential") {
    return new OrderedSequential(params);
  } else if (id == "BasicParallel") {
    return new BasicParallel();
  } else {
    throw invalid_argument(fmt::format("Algorithm {} not found", id));
  }
}

int main(int argc, char const *argv[])
{
    argh::parser cmdl;
    cmdl.add_params({"-i", "--input"}); // pre-register "input" as a param: name + value
    cmdl.add_params({"-o", "--output", "--compress-to"}); // pre-register "output" as a param: name + value
    cmdl.add_params({"-a", "--algorithm"}); // pre-register "output" as a param: name + value
    cmdl.parse(argc, argv);
    if (cmdl[{"-h", "--help"}] || cmdl.params().size()==0)
    {
        
        std::cout << "USAGE: cologra_cli [bench|color|compress] [FLAGS]" << std::endl;
        std::cout << std::endl;
        std::cout << "cologra_cli bench\t\t computes our benchmark" << std::endl;
        std::cout << std::endl;
        std::cout << "cologra_cli color -i <path_to_input_file> -o <path_to_output_file>,\t\t interprets the matrix as adjacency matrix of a graph, computes a coloring, prints the number of colors used and (if an outfile is given) writes the adjacency matrix of the colored graph" << std::endl;
        std::cout << "color, compress\t\t computes a coloring for the given matrix" << std::endl;
        std::cout << "bench\t\t prints this message" << std::endl;
        std::cout << "FLAGS:" << std::endl;
        std::cout << "-h, --help\t\t prints this message" << std::endl;
        std::cout << "-i, --input [path_to_file]\t\t prints this message" << std::endl;
        std::cout << "-o, --output, compress-to\t\t prints this message" << std::endl;
        std::cout << "-h, --help\t\t prints this message" << std::endl;
        std::cout << "-h, --help\t\t prints this message" << std::endl;
        std::cout << "HELP WANTED";
        return 0;
    }
    
    bool make_verbose = cmdl[{"-v", "--verbose"}];
    if (make_verbose){
        std::cout << "The Output is verbose" << std::endl;
    }

    if(cmdl(1) == "bench"){
        mpi::environment env(argc-1, argv+1);
        runBenchmark();
        return 0;
    }
    std::filesystem::path infile;
    if (cmdl({"-i", "--input"}) >> infile){
        std::cout << "Using matrix from "<< infile << std::endl;
    }else{
        std::cout << "No valid matrix file given, use --help for more help" << std::endl;
        return 0;
    }
    
    std::filesystem::path outfile;
    if (cmdl({"-o", "--output", "--compress-to"}) >> outfile){
        std::cout << "Writing into "<< outfile << std::endl;
    }else{
        std::cout << "No valid output file path given, not writing anything" << std::endl;
    }

    Eigen::SparseMatrix<double> matrix = readMatrixMarket(infile);
    if (make_verbose){
        std::cout << "Read in of matrix done\n";
    }
    Graph graph;

    if(cmdl["--columnIntersectionGraph"]){
        graph = columnIntersectionGraph(matrix);
    }
    else if(cmdl["--adjacencyGraph"]){
        graph = adjacencyGraph(matrix);
    }
    else{
        std::cout << "No matrix interpretation given, using Adjacency Graph" << std::endl;
        graph = adjacencyGraph(matrix);
    }
    if (make_verbose){
        std::cout << "Conversion into graph done\n";
    }


    json params = {};
    ColoringAlgorithm * algo =  createAlgorithm(cmdl({"-a", "--algorithm"}).str(), params);

    std::vector<VerticesSizeType> colorVec(boost::num_vertices(graph));
    ColorMap coloring(&colorVec.front(), get(vertex_index, graph));
    
    ColorType numColors = algo->computeColoring(graph, coloring);
    std::cout << numColors << " colors used" << std::endl;


    

    return 0;
}
