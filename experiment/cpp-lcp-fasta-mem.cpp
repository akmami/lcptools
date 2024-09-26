/**
 * @file    cpp-lcp-fasta.cpp
 * @brief   Analysis and Processing of Genomic Data
 *
 * This program is designed for in-depth analysis of genomic sequences. It reads
 * genomic data, processes it through multiple levels of analysis (defined by
 * LCP_LEVEL), and computes memory usage of the program alogn with execution times.
 *
 * The program leverages a series of custom functions and data structures, operating
 * on large volumes of data with efficiency and accuracy in focus.
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <chrono>
#include "lps.h"
#include "../utils/helper.cpp"


#define STRING_SIZE         250000000
#define MAX_CORE_COUNT      300000007
#define COMPUTE_SIZES


void process(std::string& sequence, std::vector<std::chrono::milliseconds>& durations, int (&total_core_counts)[LCP_LEVEL], int (&distinct_core_counts)[LCP_LEVEL], double (&sizes)[LCP_LEVEL], std::set<uint32_t>& distinct_cores, std::vector<lcp::lps*>& strs ) {
    
    auto start = std::chrono::high_resolution_clock::now();
    // size_t initial_table_size = lcp::hash::str_map.size();
    // size_t initial_hash_size = distinct_cores.size();
    lcp::lps *str = new lcp::lps(sequence);
    
    auto extraction_end = std::chrono::high_resolution_clock::now();
    total_core_counts[0] += str->cores->size();
    
    // using simple hash
    // std::vector<uint32_t> cores;
    // str->get_labels(cores);
    // for (const auto& core : cores) {
    //     distinct_cores.insert(core);
    // }
    // distinct_core_counts[0] += distinct_cores.size() - initial_hash_size;
    // using hash tables
    // distinct_core_counts[0] += lcp::hash::str_map.size() - initial_size;
   
    durations[0] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(extraction_end - start).count());
    #ifdef COMPUTE_SIZES
    sizes[0] += str->memsize();
    #endif

    for ( int i = 1; i < LCP_LEVEL; i++ ) {

        auto start_level = std::chrono::high_resolution_clock::now();
        // initial_table_size = lcp::hash::cores_map.size();
        // initial_hash_size = distinct_cores.size();
        
        str->deepen();
        
        auto stop_level = std::chrono::high_resolution_clock::now();
        total_core_counts[i] += str->cores->size();
        
        // using simple hash
        // cores.clear();
        // str->get_labels(cores);
        // for (const auto& core : cores) {
        //     distinct_cores.insert(core);
        // }
        // distinct_core_counts[i] += distinct_cores.size() - initial_hash_size;
        // using hash tables
        // distinct_core_counts[i] += lcp::hash::cores_map.size() - initial_table_size;
        
        durations[i] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(stop_level - start_level).count());
        #ifdef COMPUTE_SIZES
        sizes[i] += str->memsize();
        #endif
    }

    std::cout << "Length of the processed sequence: " << format_int(sequence.size()) << std::endl;
    
    strs.push_back( str );
    sequence.clear();

    str = nullptr;
}

/**
 * @brief The entry point of the program.
 *
 * The main function coordinates the entire genomic data analysis process. It 
 * initializes necessary data structures, reads input genomic sequences, and 
 * stores all lcp cores in vector to measure memory usage.
 *
 * Usage: <ExecutableName> <InputFile.fasta>
 */
int main(int argc, char **argv) {

    if (argc < 2) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] " << std::endl;
        return -1;  
    }

    std::ifstream input(argv[1]);
    if (!input.good()) {
        std::cerr << "Error opening: " << argv[1] << " . You have failed." << std::endl;
        return -1;
    }

    // variables
    std::string line;

    std::fstream genome;
    genome.open(argv[1], std::ios::in);

    std::vector<lcp::lps*> strs;
    std::vector<std::chrono::milliseconds> durations(LCP_LEVEL);
    double sizes[LCP_LEVEL] = {0};
    int total_core_counts[LCP_LEVEL] = {0};
    int distinct_core_counts[LCP_LEVEL] = {0};
    std::set<uint32_t> distinct_cores;
    
    // read file
    if ( genome.is_open() ) {  
        
        std::string sequence, id;
        sequence.reserve(STRING_SIZE);

        // initializing coefficients of the alphabet and hash tables
        lcp::init_coefficients();
        // lcp::hash::init(4000, MAX_CORE_COUNT);
                
        std::cout << "Program begins" << std::endl;

        std::cout << "str_map.capacity at the begining: " << format_int( lcp::hash::str_map.max_load_factor() * lcp::hash::str_map.bucket_count() ) << std::endl;
        
        while ( getline(genome, line) ) {

            if (line[0] == '>') {

                // process previous chromosome before moving into new one
                if (sequence.size() != 0) {
                    process( sequence, durations, total_core_counts, distinct_core_counts, sizes, distinct_cores, strs );
                }

                id = line.substr(1);
                std::cout << "Processing started for " << id << std::endl;
                continue;
                    
            }
            else if (line[0] != '>'){
                sequence += line;
            }
        }

        if (sequence.size() != 0) {
            process( sequence, durations, total_core_counts, distinct_core_counts, sizes, distinct_cores, strs );
        }

        genome.close();
    }

    std::cout << std::endl;

    std::cout << "str_map.capacity at the end: " << format_int(lcp::hash::str_map.max_load_factor() * lcp::hash::str_map.bucket_count() ) << std::endl;
    std::cout << "str_map.size at the end: " << format_int( lcp::hash::str_map.size() ) << std::endl;
    
    std::cout << std::endl;

    std::cout << "cores_map.size at the end: " << format_int( lcp::hash::cores_map.size() ) << std::endl;
    std::cout << "cores_map.load_factor at the end: " << format_double( lcp::hash::cores_map.load_factor(), 6 ) << std::endl;

    std::cout << std::endl;

    std::string sep = " & ";

    std::cout << "LCP level";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << i + 1;
    }
    std::cout << std::endl;

    // Total Cores
    std::cout << "Total Cores";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_int(total_core_counts[i]);
    }
    std::cout << std::endl;

    // Distinct Cores
    std::cout << "Distinct Cores w Table";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_int(distinct_core_counts[i]);
    }
    std::cout << std::endl;

    // Execution Time
    std::cout << "Execution Time (sec)";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(((double) durations[i].count()) / 1000);
    }
    std::cout << std::endl;

    // Total Sizes
    std::cout << "Total Sizes(GB)";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(sizes[i] / (1024.0 * 1024.0 * 1024.0));
    }
    std::cout << std::endl;

    std::cout << std::endl;
    
    std::cout << "ID: " << format_int(lcp::hash::next_id) << std::endl;
    std::cout << "Total number of cores: " << lcp::hash::str_map.size() + lcp::hash::cores_map.size() << std::endl;
    
    std::cout << std::endl;

    lcp::hash::summary();

    // cleanup
    for( std::vector<lcp::lps*>::iterator it = strs.begin(); it != strs.end(); it++ ) {
        delete (*it);
    }
    strs.clear();

    return 0;
};
