//#include "benchmark/entry.hpp"
#include "cologra/algorithms/BasicParallel.hpp"
#include "cologra/algorithms/BasicSequential.hpp"
#include "cologra/algorithms/BoostSequential.hpp"
#include "cologra/algorithms/OrderedSequential.hpp"
#include "cologra/algorithms/RandomPermutationQueue/RandomPermutationQueue.hpp"
#include "cologra/definitions.hpp"
#include "cologra/util/createAlgorithm.hpp"
#include "cologra/util/degeneracy.hpp"
#include "cologra/util/matrixIO.hpp"
#include "cologra/util/matrixToGraph.hpp"
#include <Eigen/SparseCore>
#include <argh.h>
#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

int main(int argc, char **argv)
{
    argh::parser cmdl;
    cmdl.add_params({"-i", "--input"}); // pre-register "input" as a param: name + value
    cmdl.add_params({"-o", "--output", "--compress-to"}); // pre-register "output" as a param: name + value
    cmdl.add_params({"-a", "--algorithm"}); // pre-register "algorithm" as a param: name + value
    cmdl.add_params({"--use-degeneracy"}); // pre-register "algorithm" as a param: name + value
    cmdl.parse(argc, argv);
    if (cmdl[{"-h", "--help"}] || cmdl.params().size()==0)
    {
        
        std::cout << "USAGE: cologra_cli [bench|color|compress] [FLAGS]" << std::endl;
        std::cout << std::endl;
        std::cout << "cologra_cli bench\t\t computes our benchmark" << std::endl;
        std::cout << std::endl;
        std::cout << "cologra_cli color -i <path_to_input_file> -o <path_to_output_file>,\t\t interprets the matrix as adjacency matrix of a graph, computes a coloring, prints the number of colors used and (if an outfile is given) writes the colored graph in TODO form" << std::endl;
        std::cout << "cologra_cli compress -i <path_to_input_file> -o <path_to_output_file>\t\t like color, only the file output is the compressed matrix instead of the colored graph" << std::endl;
        std::cout << "FLAGS:" << std::endl;
        std::cout << "-h, --help\t\t prints this message" << std::endl;
        std::cout << "-h, --verbose\t\t prints some stuff" << std::endl;
        std::cout << "-i, --input <path_to_file>\t\t specify input file" << std::endl;
        std::cout << "-o, --output, --compress-to <path_to_file>\t\t specify output file" << std::endl;
        std::cout << "-a, --algorithm <algo-name>\t\t which algorithm is used for the coloring" << std::endl;
        std::cout << "--use-degeneracy <k>\t\t computes the coloring in the k-degeneracy Graph. If the result is a " << std::endl;
        std::cout << "--columnIntersectionGraph, --columnwise\t\t chenges the first step by creating a CIG instead of interpreting the matrix as adjacency matrix. For compression, this option compresses columnwise" << std::endl;
        std::cout << "--adjacencyGraph\t\t chenges the first step by creating a CIG instead of interpreting the matrix as adjacency matrix" << std::endl;
        return 0;
    }
    
    bool make_verbose = cmdl[{"-v", "--verbose"}];
    if (make_verbose){
        std::cout << "The Output is verbose" << std::endl;
    }

    if(cmdl(1).str() == "bench"){
        mpi::environment env(argc, argv);       //is it safe to leave "bench" in argv unignored?
        //runBenchmark();
        std::cout << "Dependencies broken, sorry, ask Marco about it" << std::endl;
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

    if(cmdl["--columnIntersectionGraph", "--columnwise"]){
        graph = columnIntersectionGraph(matrix);
    }
    else if(cmdl["--adjacencyGraph"]){
        graph = adjacencyGraph(matrix);
    }
    else{
        if(cmdl(1).str() == "compress"){
            std::cout << "No compression direction given, using columnwise" << std::endl;
            graph = columnIntersectionGraph(matrix);
        }
        else{
            std::cout << "No matrix interpretation given, using Adjacency Graph" << std::endl;
            graph = adjacencyGraph(matrix);
        }

    }
    if (make_verbose){
        std::cout << "Conversion into graph done\n";
    }

    int k;
    if (cmdl["--use-degeneracy"]>>k){
        degeneracy_graph(graph, k);
    }


    json params = json::parse(cmdl("--params", "").str());
    ColoringAlgorithm * algo =  createAlgorithm(cmdl({"-a", "--algorithm"}).str(), params);

    
    OutType result = algo->computeColoring(graph);
    std::cout << result.first << " colors used" << std::endl;

    for (auto it = vertices(graph).first; it != vertices(graph).second; it++) {
        std::cout << result.second[*it] << " ";
    }
    std::cout << std::endl;

    //TODO: rebuild coloring from degenerate graph

    // TODO: write result into file somehow

    return 0;
}
